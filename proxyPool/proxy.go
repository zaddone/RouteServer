package proxyPool
import (
	"net"
	"io"
	"time"
	"fmt"
	"log"
)
const (
//	SCHEME [6]string= [6]string{"socks5","socks4","socks4a","https","http","socks"}
)
//var (
//	BeingUsed []*ProxyInfo
//)

type ProxyPool struct {
	ProxyList []*ProxyInfo
	ConnChan chan net.Conn
	In chan []byte
	Out chan []byte
}
func (self *ProxyPool) Replace(){
	if len(self.ProxyList) == 0 {
		self.LoadProxyList(nil)
		return
	}
	tmp:=self.ProxyList[0]
//	fmt.Println("delete",tmp.String())
	self.ProxyList = self.ProxyList[1:]
	if len(self.ProxyList) == 0 {
		self.LoadProxyList(nil)
		return
	}
	go func(){
		err := tmp.SaveDB()
		if err != nil {
			log.Println(err)
		}
	}()
}

func (self *ProxyPool) LoadProxyList (BeingUsed []*ProxyInfo) {

	list ,err := GetProxyList(false)
	if err != nil {
		return
	}
	for _,p := range BeingUsed{
		self.ProxyList =append(self.ProxyList, p)
		for i,tp := range list {
			if tp.IpInt == p.IpInt {
				list = append(list[:i],list[i+1:]...)
				break
			}
		}
	}
	self.ProxyList = append(self.ProxyList,list...)

}

func (self *ProxyPool) GetConn (addr string) (net.Conn, error) {

	if len(self.ProxyList) == 0 {
		return nil,fmt.Errorf("Found Not Proxy")
	}
	return ConnProxy(addr,self.ProxyList[0].String())

}
func (self *ProxyPool) Inits(addr string,BeingUsed []*ProxyInfo) {
	self.In = make(chan []byte,1)
	self.Out = make(chan []byte,1)
	self.ConnChan = make(chan net.Conn,10)
	self.LoadProxyList(BeingUsed)
	go self.SyncServer(addr)
	return
}
func (self *ProxyPool) SyncServer(addr string) {
	go func(){
		var conn net.Conn
		var ok bool
		for{
			conn,ok=<-self.ConnChan
			if !ok{
				return
			}
			go self.RelayHandle(conn)
		}
	}()
	var ClientConn net.Conn
	var err error
	connfunc := func(){
		for{
		//	net.DialTimeout("tcp4",addr,time.S)
			ClientConn,err = net.DialTimeout("tcp4",addr,time.Second*3)
			if err != nil {
				log.Println(err)
				time.Sleep(time.Millisecond*10)
			}else{
				return
			}

		}
	}
//	fmt.Println("run conn")
	connfunc()
	var buf [1024]byte
	var n int
//	var ok bool
//	fmt.Println("run conn server")
	for{
		val,ok:=<-self.In
		if !ok {
			return
		}
//		fmt.Println("in val",val)
//		ClientConn.SetWriteDeadline(time.Now().Add(time.Second))
		_,err = ClientConn.Write(val)
		if err != nil {
			log.Println(err)
			ClientConn.Close()
			connfunc()
		//	ClientConn.SetWriteDeadline(time.Now().Add(time.Second))
			_,err = ClientConn.Write(val)
			if err != nil {
				log.Println(err)
				//self.Out <- nil
				self.Out <-[]byte{1}
				continue
			}
		}

//		ClientConn.SetReadDeadline(time.Now().Add(time.Second))
		n,err = ClientConn.Read(buf[:])
		if err != nil {
			log.Println(err)
			ClientConn.Close()
			connfunc()
	//		ClientConn.SetReadDeadline(time.Now().Add(time.Second))
			n,err = ClientConn.Read(buf[:])
			if err != nil {
				log.Println(err)
				self.Out <-[]byte{1}
				continue
			}
		}
		self.Out <- buf[:n]
	}
}
func (self *ProxyPool) GetRemoteAddr(port string) (string,error) {

//	fmt.Println("in",port)
	self.In <- []byte(port)
	buf,ok := <-self.Out
	if !ok || len(buf) == 0 {
		return "",fmt.Errorf("GetRempteAddr Err 1")
	}
	if len(buf) == 1{
//		if buf[0] == 1 {
//			return self.GetRemoteAddr(port)
//		}
		return "",fmt.Errorf("GetRempteAddr Err 1")

	}
//	fmt.Println("out",port,string(buf))
	return string(buf),nil
}

func (self *ProxyPool) RelayHandle(conn net.Conn){
	defer conn.Close()
	_,port,err := net.SplitHostPort(conn.RemoteAddr().String())
	if err!= nil {
		log.Println(err)
		return
	}
	addr ,err := self.GetRemoteAddr(port)
	if err!= nil {
		log.Println(err)
		return
	}
	tconn,err:=self.GetConn(addr)
	if err!= nil {
		log.Println(err)
		return

//		go func(){
//			er := self.Load()
//			if er != nil {
//				log.Println(er)
//			}
//		}()
//		tconn,err = net.Dial("tcp",addr)
//		if err != nil {
//			log.Println(err)
//			return
//		}
	}
	exit:=make(chan bool,1)
	go func(){
		_,err := io.Copy(conn,tconn)
		if err != nil && err != io.EOF {
			log.Println(err)
		}
		//if exit != nil {
			exit<-true
	//	}
	}()
	go func(){
		_,err = io.Copy(tconn,conn)
		if err != nil && err != io.EOF {
			log.Println(err)
		}
	//	if exit != nil {
			exit<-true
	//	}
	}()
	<-exit
	tconn.Close()
	return

}
