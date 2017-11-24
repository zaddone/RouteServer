package ai
import(
	"fmt"
	"net"
//	"strconv"
//	"strings"
//	"log"
	"time"
)
type BitHandlefuncType func([]byte,Robot) []byte
var (
	BitAIUser map[byte]Robot
	BitHandleList map[byte]BitHandlefuncType
)
func ClearBitServer(){
	for{
		<-time.After(1*time.Hour)
		for k,v := range BitAIUser{
			if v.GetBeginTimeDiff() > 600 {
				delete(BitAIUser,k)
			}
		}
	}
}
func init(){
	BitAIUser = make(map[byte]Robot)
	BitLoadHandleFunc()
}
func BitLoadHandleFunc(){
	BitHandleList = make(map[byte]BitHandlefuncType)
	BitHandleList[255] = func(s []byte,u Robot) []byte {
		re:=HostProxyMap[u.GetHost()]
		if re != nil {
			re.Replace()
		}
		return nil
	}
	BitHandleList[10] = func(s []byte,u Robot) []byte {
		u.SetSeeList(s)
		return nil
	}
	BitHandleList[1] = func(s []byte,u Robot) []byte {
		var pro int =0
		for i:=len(s)-1;i>=0;i--{
			pro = (pro<<8) | int(s[i])
		}
		u.SetProperty(pro)
		fmt.Println("Property",pro)
		return nil
	}
	BitHandleList[0] = func(s []byte,u Robot) []byte {
//		u.Init()
		return []byte{'O', 0}
	}
}
func BitAiServer(Port string){
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
		go BitAiHandle(conn)
	}
}
func BitAiHandle(conn net.Conn){
	for {
		err := BitAiReadData(conn)
		if err != nil {
			fmt.Println(err)
			break
		}
	}
	defer conn.Close()

}
func BitAiReadData(conn net.Conn) error {
	var db [1024]byte
	n,err := conn.Read(db[0:])
	if err != nil {
		return err
	}
//	fmt.Println(db[:n])
//	_,err = conn.Write([]byte{0})
	w:=BitAiDataHandle(db[:n],conn)
	if w != nil {
		_,err = conn.Write(w)
		return err
	}
	return fmt.Errorf("p");
}
func BitAiDataHandle(data []byte,conn net.Conn) (b []byte) {
	f:=BitHandleList[data[2]]
	if f == nil {
		return b
	}
	u,k:=GetBitAIUser(data[0],conn)
	if u== nil{
		return nil
	}
//	b = append(b,k)
	b = append(b,k)
	b = append(b,f(data[3:],u)...)
	return b
}
func GetBitAIUser(key byte,conn net.Conn) (u Robot,k byte) {
	u = BitAIUser[key]

	if u == nil {
//		u.UpdateBeginTime()
		var i byte;
		for i=0;i<255;i++{
			if BitAIUser[i] ==  nil {
				u = new(Ai)
				u.Init()
				u.SetHost(conn)
				key = i
				BitAIUser[i] = u
				break
			}
		}
	}
	u.UpdateBeginTime()
	return u,key
}
