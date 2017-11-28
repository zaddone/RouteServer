package crawler
import(
	"testing"
	"fmt"
)
func TestWebStruct(t *testing.T){
	web := new(WebStruct)
	Hosts,err := web.GetHost("https://proxy.mimvp.com/free.php?proxy=in_socks")
	if err != nil {
		fmt.Println(err)
		return
	}
	for _,h := range Hosts {
		fmt.Println(h)
	}
}
