package ai
import(
	"fmt"
//	"./ai"
	"net"
	"strconv"
	"strings"
	"log"
	"time"
//	"../comm"
	//"encoding/json"
)
type HostProxy interface{
	Replace()
}
type Robot interface{
	Init()

	SetHost(conn net.Conn)
	GetHost() int

	SetLastSee(nid int,n int)
	GetLastSee() int
	Outs(isf int,No30 bool) int
	OutDiscard(num int) (o []int)
	GetDiscard() int
	SeeOuts(v int,key []int,isf int) int
	SetLastForData(n int,s string)
	GetLastForData() [2]string
	SetNowVal(nid int,n int)
	SetSumVal(n int)
	SetValid(nid int,n int)
	GetValid(nid int) int
	SetPublicDown(nid int,n int,val int)
	SetPublicSee(nid int,n int)
	InitNow(str []string)
	GetFullVal(n int) int
	SetBaseVal(n int)
//	SetLastUid(uid int)
	GetBeginTimeDiff() int64
	UpdateBeginTime()
	GetProperty() int
	SetProperty(p int)

}
type HandlefuncType func([]string,Robot) []byte
var (
	AIUser map[string]Robot
	HandleList map[int]HandlefuncType
	HostProxyMap map[int]HostProxy
)
func ClearServer(){
	for{
		<-time.After(1*time.Hour)
		for k,v := range AIUser{
			if v.GetBeginTimeDiff() > 600 {
				delete(AIUser,k)
			}
		}
	}
}
func init(){
	AIUser = make(map[string]Robot)
	LoadHandleFunc()
	HostProxyMap = make(map[int]HostProxy)

}
func LoadHandleFunc(){
	HandleList = make(map[int]HandlefuncType)
	HandleList[-10] = func(s []string,u Robot) []byte {
		re:=HostProxyMap[u.GetHost()]
		fmt.Println("-10 replace")
		if re != nil {
			re.Replace()
		}
		return []byte{1}
	}
	HandleList[-4] = func(s []string,u Robot) []byte {
		return []byte{99}
	}
	HandleList[-3] = func(s []string,u Robot) []byte {
		u.Init()
		n,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
		//	return []byte{1}
		}else{
			u.SetProperty(n)
		}
//		if u.GetBeginTimeDiff() > 300 {
//			u.UpdateBeginTime()
//			return []byte{2}
//		}
		return []byte{1}
		//game over
		//return []byte{3}
	}
	HandleList[-2] = func(s []string,u Robot) []byte {
		u.Init()
		return []byte{1}
	}
	HandleList[-1] = func(s []string,u Robot) []byte {
		u.Init()
		return []byte{'O', 0}
	}
	HandleList[101] = func(s []string,u Robot) []byte {
		int_s := make([]int,len(s))
		var err error
		for i,_s := range s {
			int_s[i],err = strconv.Atoi(_s)
			if err != nil {
				fmt.Println(err)
				return []byte{1}
			}
		}
		u.SetValid(int_s[0],int_s[1])
		return []byte{1}
	}
	HandleList[90] = func(s []string,u Robot) []byte {
		int_s := make([]int,len(s))
		var err error
		for i,_s := range s {
			int_s[i],err = strconv.Atoi(_s)
			if err != nil {
				fmt.Println(err)
				return []byte{1}
			}
		}
		u.SetNowVal(int_s[0],int_s[1])
		return []byte{1}
	}
	HandleList[91] = func(s []string,u Robot) []byte {
		n,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
			return []byte{1}
		}
		u.SetSumVal(n)
		return []byte{1}
	}
	HandleList[92] = func(s []string,u Robot) []byte {
		int_s := make([]int,len(s))
		var err error
		for i,_s := range s {
			int_s[i],err = strconv.Atoi(_s)
			if err != nil {
				fmt.Println(err)
				return []byte{1}
			}
		}
		u.SetPublicDown(int_s[0],int_s[1],int_s[2])
		return []byte{1}
	}
	HandleList[93] = func(s []string,u Robot) []byte {
		if u.GetValid(3) != -1 {
			int_s := make([]int,len(s))
			var err error
			for i,_s := range s {
				int_s[i],err = strconv.Atoi(_s)
				if err != nil {
					fmt.Println(err)
					return []byte{1}
				}
			}
			u.SetPublicSee(int_s[0],int_s[1])
		}
		return []byte{1}
	}
	HandleList[94] = func(s []string,u Robot) []byte {
		n,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
			return []byte{1}
		}
		fmt.Println(94,n)
		u.SetBaseVal(n)
		return []byte{1}
	}
	HandleList[95] = func(s []string,u Robot) []byte {
		u.Init()
		u.InitNow(s)
		outs := u.OutDiscard(3)
		sends := fmt.Sprintf("%d",len(outs))
		for _,o := range outs {
			sends = fmt.Sprintf("%s %d",sends,o)
		}
		//u.SetPublicSee(int_s[0],int_s[1])
		return []byte(sends)
	}
	HandleList[96] = func(s []string,u Robot) []byte {
		u.Init()
		nowVal,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
			return []byte{1}
		}
		u.SetNowVal(3,nowVal)

		u.InitNow(s[1:])
		a := u.GetValid(3)
		sends := fmt.Sprintf("1 %d",a)
		return []byte(sends)
	}
	HandleList[97] = func(s []string,u Robot) []byte {

		nowVal,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
			return []byte{1}
		}
		u.InitNow(s[1:])
		n :=u.Outs(u.GetFullVal(nowVal),false)
		sends := fmt.Sprintf("1 %d",n)
		return []byte(sends)
	}
	HandleList[98] = func(s []string,u Robot) []byte {
		nowVal,err := strconv.Atoi(s[0])
		if err != nil {
			fmt.Println(err)
			return []byte{1}
		}
		u.InitNow(s[1:])
		n :=u.Outs(u.GetFullVal(nowVal),true)
		sends := fmt.Sprintf("1 %d",n)
		return []byte(sends)
	}
	HandleList[99] = func(s []string,u Robot) []byte {

		ns:=make( []int,len(s))
		for i,_s := range s {
			n,err := strconv.Atoi(_s)
			if err != nil {
				fmt.Println(err)
				return []byte{1}
			}
			ns[i] = n
		}
		val := u.SeeOuts(u.GetLastSee(),ns[1:],u.GetFullVal(ns[0]))
		sends := fmt.Sprintf("1 %d",val)
		return []byte(sends)

	}
	HandleList[100] = func(s []string,u Robot) []byte {
		u.InitNow(s)
		return []byte{1}
	}
}
func AiServer(Port string){
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
		go AiHandle(conn)
	}
}
func GetAIUser(key string,conn net.Conn) (u Robot) {
	u = AIUser[key]

	if u == nil {
		u = new(Ai)
		u.Init()
		u.SetHost(conn)
//		u.UpdateBeginTime()
		AIUser[key] = u
	}
	u.UpdateBeginTime()
	return u
}
func AiDataHandle(data string,conn net.Conn) []byte {

	if len(data) == 1 {
		return []byte{1}
	}
	str := strings.Split(data," ")
//	fmt.Println(str)
	key_1,err := strconv.Atoi(str[1])
	if err != nil {
		return []byte{1}
	}

	u := GetAIUser(str[0],conn)
	if key_1>0{
		la := u.GetLastForData()
		if la[0] == data {
			return []byte(la[1])
		}
	}

	u.SetLastForData(0,data)
	u.SetLastForData(1,string([]byte{1}))
	f:=HandleList[key_1]
	if f == nil {
		return []byte{1}
	}
	sen:=f(str[2:],u)
	u.SetLastForData(1,string(sen))
//	fmt.Println("back",string(sen))
	return sen

}
func AiReadData(conn net.Conn) error {
	var db [1024]byte
	n,err := conn.Read(db[0:])
	if err != nil {
		return err
	}
	_,err = conn.Write(AiDataHandle(string(db[:n]),conn))
	return err
}
func AiHandle(conn net.Conn){
	for {
		err := AiReadData(conn)
		if err != nil {
			fmt.Println(err)
			break
		}
	}
	defer conn.Close()

}
func ConsoleHandle(conn net.Conn){
	defer conn.Close()
	var putOut string = ""
	for k,v :=range AIUser{
		putOut =fmt.Sprintf("%s%s %d %d\n",putOut,k,v.GetProperty(),v.GetBeginTimeDiff())
	}
	_,err := conn.Write([]byte(putOut))
	if err != nil {
		log.Println(err)
	}
}
