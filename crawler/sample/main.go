package main
import(
	"../../crawler"
	"fmt"
	"net"
	"time"
)
func main(){
	web := new(crawler.WebStruct)
	Hosts,err := web.GetHost("https://proxy.mimvp.com/free.php?proxy=in_socks")
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println(len(Hosts))
	for _,h := range Hosts {
		scannPort(h)
	}
}
func scannPort(host string){
	var addr string
	waitPool := make(chan bool,1024)
	f := func(_i int){
		addr = fmt.Sprintf("%s:%d",host,_i)
		conn,err := net.DialTimeout("tcp",addr,time.Second*3)
		if err == nil {
			fmt.Println(addr)
			conn.Close()
			// send
		}
		<-waitPool
	}
	for i:=1;i<65536;i++{
		fmt.Printf("%d\r",i)
		waitPool <- true
		go f(i)
	}
}
