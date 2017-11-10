package main
import(
	"net"
	"flag"
	"runtime"
	"log"
	"fmt"
//	"strconv"
	"strings"
	"os"
	"./goini"
	proxy "./proxyPool"
	"./ai"
	"./scann"
	_ "./go-sqlite3"
	"./comm"
)
type ConnHandle func(conn net.Conn)
var (
	IniFile = flag.String("F","./config.ini","Ini File")
	Server = flag.Bool("Server",false,"Server mode")

	//MapHandle [3]ConnHandle
	ListHandle [3]ConnHandle
	ProxyPool map[int]*proxy.ProxyPool
	ClientPort string
//	PublicLock sync.Mutex
)

func ListenPort (port string,handle ConnHandle){
	lis,err := net.Listen("tcp4",port)
	if err != nil {
		panic(err)
	}
	defer lis.Close()
	for {
		conn,err := lis.Accept()
		if err != nil {
			log.Println("accept:",err)
			continue
		}
		handle(conn)
	}
}

func ProxyHandle(conn net.Conn){
	defer conn.Close()

	var err error

	var buf [1024]byte
	var n int
	n,err = conn.Read(buf[:])
	if err!= nil {
		log.Println(err)
		return
	}
//	ClearProxyPool := func(conn net.Conn){
//		localhost,_,err := net.SplitHostPort(conn.RemoteAddr().String())
//		if err != nil {
//			log.Println(err)
//			return
//		}
//		fmt.Println("clearProxy",localhost)
//		pr := ProxyPool[comm.StringIpToInt(localhost)]
//		if pr== nil {
//			return
//		}
//		pr.Replace()
//
//	}
	if n == 1 {
		if buf[0] == 100 {
			err =  scann.GetScannIP(conn)
		}else if buf[0] == 101 {
			err =  scann.GetScannPort(conn)
//		}else if buf[0] == 98 {
//			ClearProxyPool(conn,false)
//			return
//		}else if buf[0] == 99 {
//			ClearProxyPool(conn)
//			return
		}else  if buf[0] == 90{
			ai.ConsoleHandle(conn)
		//	_,err = conn.Write([]byte{1})
		}
		fmt.Println(err)
		if err != nil {
			log.Println(err)
		}
		return
	}else{
		var pi proxy.ProxyInfo
		pi.Init(string(buf[:n]),"",-1)
		go func(){
			err := pi.Check()
			if err == nil {
				err = pi.SaveDB()
			}
			if err != nil {
				log.Println(err)
			}
		}()
	}
	return
}


func init(){
	ProxyPool = make(map[int]*proxy.ProxyPool)
//	MapHandle = make(map[string]ConnHandle)
	ListHandle[0] = func(conn net.Conn){
		go ProxyHandle(conn)
	}
	ListHandle[1] = func(conn net.Conn){
//		fmt.Println(conn.RemoteAddr())
		localhost,_,err := net.SplitHostPort(conn.RemoteAddr().String())
		if err!= nil {
			conn.Close()
			return
		}
		ips := comm.StringIpToInt(localhost)
		pr := ProxyPool[ips]
		if pr == nil {
			pr = new(proxy.ProxyPool)
			pr.Inits(fmt.Sprintf("%s:%s",localhost,ClientPort),func()(li []*proxy.ProxyInfo) {
			for _,p := range ProxyPool {
				li = append(li,p.ProxyList[0])
			}
			return

}())
			ai.HostProxyMap[ips] = pr
			ProxyPool[ips] = pr
		}
		pr.ConnChan<-conn
//		log.Println(localhost,len(pr.ConnChan))
	}
	ListHandle[2] = func(conn net.Conn){
		go ai.AiHandle(conn)
	}
}

func main(){
	flag.Parse()
	runtime.GOMAXPROCS(runtime.NumCPU())
	var goI *goini.Config
	var err error
	_,err = os.Stat(*IniFile)
	if err != nil  {
		goI =goini.SetConfig(*IniFile)
		goI.SetValue("Client","port","2000")
		goI.SetValue("Host","ListenPort",":3330,:3332,:3333")
		fmt.Println("new ini")
	}else{
		goI =goini.SetConfig(*IniFile)
	}
	ClientPort = goI.GetValue("Client","port")
	fmt.Println("Client port",ClientPort)
//	pr :=new(proxy.ProxyInfo)
//	pr.Lo = &PublicLock

	for i,v := range strings.Split(goI.GetValue("Host","ListenPort"),","){
		if i >= len(ListHandle){
			break
		}
		fmt.Println("Listen port",v)
		go ListenPort(v,ListHandle[i])
	}
	if *Server == false {
		var cmd string
		for{
			fmt.Scanf("%s",&cmd)
			var pi proxy.ProxyInfo
			pi.Init(cmd,"",-1)
			err = pi.Check()
			if err == nil {
				err = pi.SaveDB()
			}
			fmt.Println(cmd,err)
		}
	}else{
		go proxy.CheckProxyPoolServer()
		ai.ClearServer()
	}
}
