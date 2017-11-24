package robot
import(
	"fmt"
	"net"
	"time"
	"../mjai"
	"../comm"
	"log"
)
type DZPK interface{
}
type HostProxy interface{
	Replace()
}
type UserBase struct {
	Property int
	Host int
	BeginTime int64
	Scmj SCMJ
	Dzpk DZPK
	LastForData [2][]byte
}
func(self *UserBase) Init(conn net.Conn){
	Host,_,err:= net.SplitHostPort(conn.RemoteAddr().String())
	if err != nil {
		log.Fatal(err)
	}
	self.Host = comm.StringIpToInt(Host)
	self.Scmj = new(mjai.Ai)
	self.Scmj.Init()
}
type HandlefuncType func(b []byte,u *UserBase) []byte
var (
	HostMap map[byte]*UserBase
	MethodLib map[uint16]HandlefuncType
	HostProxyMap map[int]HostProxy
)
func init(){
	HostMap = make(map[byte]*UserBase)
	HostProxyMap = make(map[int]HostProxy)
	LoadMethodLib()
}
func ConsoleHandle(conn net.Conn){
	defer conn.Close()
	var putOut string = ""
	for k,v :=range HostMap{
		putOut =fmt.Sprintf("%s%d %d %d\n",putOut,k,v.Property,time.Now().Unix() - v.BeginTime)
	}
	_,err := conn.Write([]byte(putOut))
	if err != nil {
		log.Println(err)
	}
}
func ClearServer(){
	for{
		<-time.After(1*time.Hour)
		for k,v :=range HostMap{
			if (time.Now().Unix() - v.BeginTime) > 600 {
				delete(HostMap,k)
			}
		}
	}
}
func LoadMethodLib(){
	MethodLib=make(map[uint16]HandlefuncType)
	LoadScmjFuncLib()
	key := int(ScmjId)<<8
	for k,v := range ScmjFuncLib{
		MethodLib[uint16(key|int(k))] = v
	}
	MethodLib[255] = func(b []byte,u *UserBase) []byte{
		re:=HostProxyMap[u.Host]
		if re != nil {
			re.Replace()
		}
		return nil
	}
	MethodLib[1] = func(s []byte,u *UserBase) []byte {
		u.Property =comm.ByteToInt(s)
		fmt.Println("Property",u.Property)
		return nil
	}
	MethodLib[0] = func(s []byte,u *UserBase) []byte {
		return []byte{'O', 0}
	}
}
func compByte(a ,b []byte) bool {
	if len(a) != len(b) {
		return false
	}
	for i,_a := range a {
		if b[i] != _a {
			return false
		}
	}
	return true
}
func GetRecvData(db []byte,conn net.Conn) (b []byte) {

	if len(db)<3 {
		return nil
	}
	ke:=uint16(int(db[1])<<8 |int(db[2]))
//	fmt.Println("ke",ke)
	f := MethodLib[ke]
	if f== nil{
		b = append(b,db[0])
		return b
	//	return nil
	}
	u,k := GetHost(db[0],conn)
	if u == nil{
		return nil
	}
	if !compByte(u.LastForData[0],db[3:]) {
		u.LastForData[0] = db[3:]
		u.LastForData[1] = f(u.LastForData[0],u)
	}
	b = append(b,k)
	b = append(b,u.LastForData[1]...)
	return b

}
func GetHost(key byte,conn net.Conn) (u *UserBase,k byte) {
	u = HostMap[key]

	if u == nil {
		var i byte
		for i=0;i<255;i++{
			if HostMap[i] ==  nil {
				u = new(UserBase)
				u.Init(conn)
				HostMap[i] = u
				key = i
				break
			}
		}
	}
	u.BeginTime = time.Now().Unix()
	return u,key
}
func Server(Port string){
	lis,err := net.Listen("tcp4",Port)
	if err != nil {
		panic(err)
	}
	defer lis.Close()
	for {
		conn,err := lis.Accept()
		if err != nil {
			fmt.Println("accept:",err)
			continue
		}
		go Handle(conn)
	}
}
func Handle(conn net.Conn) {
	for {
		var db [1024]byte
		n,err := conn.Read(db[0:])
		if err != nil {
			break
		}
	//	fmt.Println(db[:n])
		w:=GetRecvData(db[:n],conn)
		if w == nil {
			break
		}
//		fmt.Println("recv",w)
		_,err = conn.Write(w)
		if err != nil {
			break
		}
	}
	conn.Close()

}
