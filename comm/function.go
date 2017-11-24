package comm
import(
	"strings"
	"strconv"
	"bytes"
)

func IpIntToString(ipInt int) string {
    ipSegs := make([]string, 4)
    var len int = len(ipSegs)
    buffer := bytes.NewBufferString("")
    for i := 0; i < len; i++ {
        tempInt := ipInt & 0xFF
        ipSegs[len-i-1] = strconv.Itoa(tempInt)
        ipInt = ipInt >> 8
    }
    for i := 0; i < len; i++ {
        buffer.WriteString(ipSegs[i])
        if i < len-1 {
            buffer.WriteString(".")
        }
    }
    return buffer.String()
}
func StringIpToInt(ipstring string) int {
    ipSegs := strings.Split(ipstring, ".")
    var ipInt int = 0
    var pos uint = 24
    for _, ipSeg := range ipSegs {
        tempInt, _ := strconv.Atoi(ipSeg)
        tempInt = tempInt << pos
        ipInt = ipInt | tempInt
        pos -= 8
    }
    return ipInt
}
func ByteToInt(s []byte) (pro int) {
	pro=0
	for i:=len(s)-1;i>=0;i--{
		pro = (pro<<8) | int(s[i])
	}
	return pro
}
