package proxyPool
import(
	"./ProxyClient"
	"database/sql"
	"fmt"
	"sync"
	"net"
	"net/url"
	"log"
	"time"
	"strconv"
	"../comm"
	"os"
)
var (
	Lo sync.Mutex
	SCHEME []string= []string{"socks5","socks4","socks4a","https","http","socks"}
)
const (
	dbPath = "/db/proxy_pool.db"
	MaxTime = 7*24*3600*1000
)
type ProxyInfo struct {
//	Ip string
	Port int
	Scheme int
	Au string
	IpInt int
	Url string
	Order int64

}
func (self *ProxyInfo) Init(hostPort string,au string,scheme int) error {
	host,port,err := net.SplitHostPort(hostPort)
	if err != nil {
		return err
	}
	self.Port,err = strconv.Atoi(port)
	if err!= nil {
		return err
	}
	if self.Port<0 || self.Port > 65535 {
		return fmt.Errorf("port is %d,err",self.Port)
	}
	self.Au = au
	self.IpInt =comm.StringIpToInt(host)
//	self.Port = _port
	self.Scheme = scheme
//	return self.SaveDB()
//	if err == nil {
//		log.Println("add",hostPort)
//	}
	return nil

}
func (self *ProxyInfo) DeleteDB() (err error) {
	Lo.Lock()
	defer Lo.Unlock()
	db,err := sql.Open("sqlite3",dbPath)
	if err != nil {
		return err
	}
	defer db.Close()
	_,err = db.Exec("DELETE FROM ProxyPool WHERE Ip = ?",self.IpInt)
	return err
}
func (self *ProxyInfo)Check() (err error) {
	var conn net.Conn = nil
	if self.Scheme >= 0 && self.Scheme <6 {
		self.Order = time.Now().UnixNano()
		conn,err = ConnProxy("",self.String())
		if err != nil {
			return err
		}else{
			conn.Close()
			self.Order = time.Now().UnixNano() - self.Order
		}
	}else{
		for i,_ := range SCHEME {
			self.Scheme = i
			self.Order = time.Now().UnixNano()
			conn,err = ConnProxy("",self.String())
			self.Url = ""
			if err == nil {
				conn.Close()
				self.Order = time.Now().UnixNano() - self.Order
				break
			}
		}
		if err != nil {
			self.Order = 0
			return err
		}
	}
	return nil
}

func (self *ProxyInfo)SaveDB() (err error) {

	_,err = os.Stat(dbPath)
	if err != nil {
		CreateDB()
		return err
	}
	Lo.Lock()
	defer Lo.Unlock()
	db,err := sql.Open("sqlite3",dbPath)
	if err != nil {
		return err
	}
	defer db.Close()
	_,err = db.Exec("insert into ProxyPool(Ip, Port,Scheme,Start,Au,Ord,Time) values(?, ?,?,?,?,?,?)",self.IpInt,self.Port,self.Scheme,true,self.Au,self.Order,time.Now().Unix())
	if err != nil {
		log.Println("update",err)
		_,err = db.Exec("UPDATE ProxyPool SET Port = ?,Scheme = ?,Start = ?,Au=?, Ord=?,Time = ? WHERE Ip = ? ",self.Port,self.Scheme,true,self.Au,self.Order,time.Now().Unix(),self.IpInt)
	}
	
	return err
//	return fmt.Errorf("order is <0")

}
func (self *ProxyInfo)String() string{
	if self.Url == ""{
		self.Url = fmt.Sprintf("%s://%s%s:%d",SCHEME[self.Scheme],self.Au,comm.IpIntToString(self.IpInt),self.Port)
	}
	return self.Url
}
func CreateDB() {
	db,err := sql.Open("sqlite3",dbPath)
	if err != nil {
		log.Fatal(err)
		return
	}
	defer db.Close()
	sqlCreate := "CREATE TABLE ProxyPool (Ip INTEGER PRIMARY KEY NOT NULL,Port INTEGER NOT NULL,Scheme INTEGER NOT NULL,Start BLOB NOT NULL,Au TEXT,Ord INTEGER,Time INTEGER);"
	_,err = db.Exec(sqlCreate)
	if err != nil {
		log.Fatal(err)
	//	log.Println(err)
	}
	return
}
func GetProxyList(isAll bool) (list []*ProxyInfo,err error) {

	_,err = os.Stat(dbPath)
	if err != nil {
		CreateDB()
		return nil,err
	}
	Lo.Lock()
	defer Lo.Unlock()
	db,err := sql.Open("sqlite3",dbPath)
	if err != nil {
		return nil,err
	}
	defer db.Close()
	var _sql string
	if isAll {
		_sql="SELECT Ip,Port,Scheme,Au FROM ProxyPool WHERE Ord = 0"
	}else{
		_sql="SELECT Ip,Port,Scheme,Au FROM ProxyPool WHERE Ord <> 0 ORDER BY Ord ASC"
	}
	rows,err := db.Query(_sql)
	if err != nil {
		return nil,err
	}
	var er error
	for rows.Next() {
		tmpPr := new(ProxyInfo)
		er = rows.Scan(&tmpPr.IpInt,&tmpPr.Port,&tmpPr.Scheme,&tmpPr.Au)
		if er != nil {
			log.Println(er)
			break
		}
		list = append(list,tmpPr)
	}
	rows.Close()

	return list,err

}
func LoadFreUsedAddr()string {
	return "117.175.92.40:80"
}
func ConnProxy(addr string,_proxy string) (net.Conn,error){
	if addr == "" {
		addr = LoadFreUsedAddr()
	}
	pr,err := url.Parse(_proxy)
	if err != nil {
		return nil,err
	}
	dial,err := proxyclient.NewClient(pr)
	if err != nil {
		return nil,err
	}
	return dial.Dial("tcp",addr)

}

func CheckProxyPoolServer(){
	for {
		CheckDB()
		<-time.After(time.Hour)
	}
}

func CheckDB(){
	list,err := GetProxyList(true)
	if err != nil {
		log.Println(err)
		return
	}
	for _,pr := range list {
		if err != pr.Check() {
			err = pr.DeleteDB()
		}else{
			err = pr.SaveDB()
		}
		if err != nil {
			fmt.Println(err)
		}
	}
}
