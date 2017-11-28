package crawler
import(
	"net/http"
	"io/ioutil"
	"fmt"
	"compress/gzip"
	"io"
	"regexp"
)
var(
	myExp=regexp.MustCompile(`((?:(?:25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d)))\.){3}(?:25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d))))`)
)
type WebStruct struct {

	Client *http.Client
	Header http.Header

}
func (self *WebStruct) GetHost(path string) (Hosts []string,err error) {

	self.Header = make(http.Header)
	self.Header.Add("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8")
	self.Header.Add("Accept-Encoding","gzip, deflate, br")
	self.Header.Add("Accept-Language","zh,zh-CN;q=0.9")
	self.Header.Add("Connection","Keep-Alive")
	self.Header.Add("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/62.0.3202.94 Chrome/62.0.3202.94 Safari/537.36")
	self.Client = new(http.Client)

	Req,err := http.NewRequest("GET",path,nil)
	if err != nil {
		return nil,err
	}
	Req.Header = self.Header
	res,err := self.Client.Do(Req)
	if err != nil {
		return nil,err
	}
	defer res.Body.Close()
//	fmt.Println(res.StatusCode)
	if res.StatusCode != 200 {
//		b,err:=ioutil.ReadAll(res.Body)
//		fmt.Println(string(b),err)
		return nil,fmt.Errorf("%d",res.StatusCode)
	}
	var reader io.ReadCloser
	switch res.Header.Get("Content-Encoding") {
		case "gzip":
			reader, _ = gzip.NewReader(res.Body)
			defer reader.Close()
		default:
			reader = res.Body
	}

	b,err:=ioutil.ReadAll(reader)
	db:= myExp.FindAll(b,-1)

	for _,_db:= range db {
		Hosts = append(Hosts,string(_db))
	}
	return Hosts,err

}
