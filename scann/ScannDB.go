package scann
import (
//	"net"
	"net/http"
//	_ "../go-sqlite3"
	"database/sql"
	"fmt"
	"sync"
	"strings"
	"bufio"
	"math/rand"
	"time"
	"io"
	"os"
	"../comm"
)

const(
	scannIPdb = "/db/scannIP.db"
	scannPortdb = "/db/scannPort.db"

	IPUrl = "http://ips.chacuo.net/down/t_txt=p_BJ"
)
var(
	IPMutex sync.Mutex
	PortMutex sync.Mutex
)
func CreateScannPortDB() (*sql.DB,error) {
	db,err := sql.Open("sqlite3",scannPortdb)
	if err == nil{
		_,err = db.Exec("CREATE TABLE `scannPort` (`port` INTEGER PRIMARY KEY NOT NULL,`order` INTEGER)")
		if err != nil {
			panic(err)
		}
		tx, err := db.Begin()
		if err != nil {
			panic(err)
		}
		stmt, err := tx.Prepare("INSERT INTO `scannPort` (`port`,`order`) values (?,?)")
		if err != nil {
			panic(err)
		}
		defer stmt.Close()
		_, err = stmt.Exec(53,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(23,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(443,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(8080,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(1080,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(80,0)
		if err != nil {
			panic(err)
		}
		_, err = stmt.Exec(22,0)
		if err != nil {
			panic(err)
		}
		tx.Commit()
	}
	return db,err
}
func AddPortDB(port int) error {
	PortMutex.Lock()
	defer PortMutex.Unlock()
	db,err := sql.Open("sqlite3",scannPortdb)
	defer db.Close()
	row := db.QueryRow("SELECT `order` FROM `scannPort` WHERE `port`= ?",port)
	var order int
	err  = row.Scan(&order)
	if err!= nil {
		_,err = db.Exec("INSERT INTO `scannPort` (`port`,`order`) values (?,?)",port,1)
	}else{
		_,err = db.Exec("UPDATE `scannPort` SET `order`=? WHERE `port` = ? ",order+1,port)
	}
	return err
}
func CreateScannIPDB() (*sql.DB,error) {
	db,err := sql.Open("sqlite3",scannIPdb)
	if err == nil{
		_,err = db.Exec("CREATE TABLE `scannIP` (`begin` INTEGER PRIMARY KEY NOT NULL,`end` INTEGER NOT NULL,`time` INTEGER)")
		if err != nil {
			panic(err)
			db.Close()
		}
		err = UpdateIPDB(db)
		if err != nil {
			panic(err)
			db.Close()
		}
	}
	return db,err
}

func UpdateIPDB(db *sql.DB) error {

	req,err := http.NewRequest("GET",IPUrl,nil)
	if err != nil {
		return err
	}
	client:= new(http.Client)
	res,err := client.Do(req)
	if err != nil {
		return err
	}
	defer res.Body.Close()
	if res.StatusCode != 200 {
		return fmt.Errorf("%d",res.StatusCode)
	}

	tx, err := db.Begin()
	if err != nil {
		return err
	}
	stmt, err := tx.Prepare("insert into `scannIP`(`begin`, `end`,`time`) values(?, ?,?)")
	if err != nil {
		return err
	}
	defer stmt.Close()
	rd := bufio.NewReader(res.Body)
	for{
		line,err := rd.ReadString('\n')
		lis :=strings.Split( strings.TrimSpace(line),"	")
		fmt.Println(lis,len(lis))
		if len(lis) ==3 {
			_, err = stmt.Exec(comm.StringIpToInt(lis[0]),comm.StringIpToInt(lis[1]),0)
			if err != nil {
				panic(err)
			}
		}
		if err != nil {
			break
		}
	}
	tx.Commit()
	return nil

}
func GetScannIP(w io.Writer) error {
	IPMutex.Lock()
	defer IPMutex.Unlock()
	_,err := os.Stat(scannIPdb)
	var db *sql.DB
	if err != nil {
		fmt.Println("new ip db")
		db,err = CreateScannIPDB()
		if err != nil {
			panic(err)
		}
	}else{
		fmt.Println("open ip db")
		db,err = sql.Open("sqlite3",scannIPdb)
		if err != nil {
			panic(err)
		}
	}
	defer db.Close()
	rows,err := db.Query("SELECT `begin`,`end`,`time` FROM `scannIP` order by `time` ASC limit 20")
	if err != nil {
		panic(err)
	}
	defer rows.Close()
	var list [20][2]int
	var Ti int
	zerolist:= 0
	i:=0
	for rows.Next() {
		err = rows.Scan(&list[i][0],&list[i][1],&Ti)
		if err !=nil {
			break
		}
		if Ti == 0 {
			zerolist = i
		}
		i++
	}
	fmt.Println("100",i)
	var begin int
	if zerolist == 0 {
		begin = list[0][0]
		_,err= w.Write([]byte(fmt.Sprintf("%d_%d",begin,list[0][1])))
	}else{
	//	zerolist ++
		ra := rand.New(rand.NewSource(time.Now().UnixNano()))
		v:=list[ra.Intn(zerolist)]
		begin = v[0]
		_,err =w.Write([]byte(fmt.Sprintf("%d_%d",begin,v[1])))
	}
	if err != nil {
		return err
	}

	_,err = db.Exec("UPDATE `scannIP` SET `time`=? WHERE `begin` = ? ",time.Now().Unix(),begin)
	return err
}
func GetScannPort(w io.Writer) error {

	PortMutex.Lock()
	defer PortMutex.Unlock()
	var db *sql.DB
	_,err := os.Stat(scannPortdb)
	if err != nil {
		db,err = CreateScannPortDB()
	}else{
		db,err = sql.Open("sqlite3",scannPortdb)
	}
	if err!= nil {
		panic(err)
	}
	defer db.Close()

	rows,err := db.Query("SELECT `port` FROM `scannPort` order by `order` DESC")
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	wf := bufio.NewWriter(w)
//	var str string = ""
	var _port int
	for rows.Next() {
		err = rows.Scan(&_port)
		if err != nil {
			panic(err)
		}
		_,err = wf.WriteString(fmt.Sprintf("%d\n",_port))
		if err != nil {
			panic(err)
		}
	}
	err = wf.WriteByte('\n')
	if err != nil {
		panic(err)
	}
//	_,err = w.Write([]byte(str))
	return wf.Flush()

}
