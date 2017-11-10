package ai
import(
	"testing"
	"fmt"
	"math/rand"
	"time"
)
func Test_Ai(t *testing.T){
	ra := rand.New(rand.NewSource(time.Now().Unix()))
	var grs [10]*MJGrain
	for i,_ := range grs{
		grs[i] = &MJGrain{H:ra.Intn(3),N:ra.Intn(9),O:[]int{ra.Intn(2),ra.Intn(2),ra.Intn(2)}}
		fmt.Println(grs[i])
	}
	fmt.Println("sort----------------")
	SortGrainArrs(grs[:],0,true,false,true)
	for _,g := range grs {
		fmt.Println(g)
	}
}
