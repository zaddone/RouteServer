package mjai
import(
//	"strconv"
	"time"
	"fmt"
	"net"
	"log"
	"../comm"
)
type MJGrain struct {
	H int // 0 1 2
	N int //0--8

	O []int
//	O1 int
}
func SortGrain(grs []*MJGrain,i int,o int) {

	if i == 0 {
		return
	}
	I := i-1
	if grs[i].O[o] < grs[I].O[o] {
		grs[i],grs[I] = grs[I],grs[i]
	}
	SortGrain(grs,I,o)

}
func SortdGrain(grs []*MJGrain,i int,o int) {

	if i == 0 {
		return
	}
	I := i-1
	if grs[i].O[o] > grs[I].O[o] {
		grs[i],grs[I] = grs[I],grs[i]
	}
	SortdGrain(grs,I,o)

}
func SortGrainArrs(grs []*MJGrain,o int,isd... bool ) *MJGrain {

	if o >= len(grs[0].O) || len(grs)== 1 || o >= len(isd) {
		return grs[0]
	}
	if isd[o] {
		for i,_ := range grs {
			SortdGrain(grs,i,o)
		}
	}else{
		for i,_ := range grs {
			SortGrain(grs,i,o)
		}
	}
	if grs[0].O[o] != grs[1].O[o] {
		return grs[0]
	}
	_i :=2
	for i,g := range grs[2:]{
		if g.O[o] != grs[0].O[o] {
			_i += i
			break
		}
	}
//	grs = grs[:_i]
	o ++
	return SortGrainArrs(grs[:_i],o,isd...)

}


type UserPublic struct {

	Now [3][9]int
	See [3][9]int
	Down [3][9]int
	Inv  int
	IsF  bool
	LastSee int
	NowVal int
	TmpNowVal int
	//LogData [][]byte
}
func (self *UserPublic) Init(){
	self.IsF = false
	self.Inv = -1
	self.LastSee = -1
	self.NowVal = 0
	for i,d := range self.See {
		for j,_ := range d {
			self.See[i][j] = 0
			self.Down[i][j] = 0
			self.Now[i][j] = 0
		}
	}
}
func (self *UserPublic) SetNow( str []byte) (yu []int){
	for i,d := range self.Now{
		for j,_ := range d {
			self.Now[i][j] = 0
		}
	}
	var n int
	for _,s := range str {
		n = int(s)
		if n < 27 {
			self.Now[n/9][n%9]++
		}else{
			yu = append(yu,n)
		}
	}

	return yu
}
type Ai struct {
	Public [4]*UserPublic
	BaseVal int
	SumVal int
	LastPublicId  int
	LastForData [2][]byte
	Blocks [3]*NewBlock
	BlocksMap map[int]*NewBlock
	Room  int
	BeginTime int64

	Property int

	Host int

	SeeList [][]byte

}
func (self *Ai) SetSeeList(b []byte){
	n:=int(b[0])
	if b[2]< 27 {
		self.SetPublicSee(n,int(b[2]))
	}else{
		le := len(self.SeeList)-1
		var lastDB []byte = nil
		for i:=le;i>=0;i--{
			if self.SeeList[i][0] != b[0] && self.SeeList[i][2]<27 {
				lastDB = self.SeeList[i]
				break
			}
		}
		if lastDB != nil{
			if b[1] == lastDB[1] {
				self.SetPublicDown(n,int(lastDB[2]),2)
			}
		}
	}
	self.SetNowVal(n,comm.ByteToInt(b[3:]))
	self.SetSumVal(int(b[1]))
	self.SeeList = append(self.SeeList,b)
}
//func (self *Ai) Begin1(b []byte) {
//	self.Init()
//	return nil
//}

func (self *Ai) SetHost(conn net.Conn){
	Host,_,err:= net.SplitHostPort(conn.RemoteAddr().String())
	if err != nil {
		log.Fatal(err)
	}
	self.Host = comm.StringIpToInt(Host)

}
func (self *Ai) GetHost() int {
	return self.Host
}
func (self *Ai) Init() {
	for i,p := range self.Public {
		if p == nil {
			p = new(UserPublic)
			self.Public[i] = p
		}
		p.Init()
	}
//	var d [2]string
//	self.LastForData = d
	self.LastPublicId = -1
//	self.BaseVal = 0
	if self.BlocksMap == nil {
		self.BlocksMap = make(map[int]*NewBlock)
	}
//	self.BlocksMap = make(map[int]*Block)
//	self.Room = -1
	self.SeeList = nil

}
func (self *Ai)GetProperty() int {
	return self.Property
}
func (self *Ai)SetProperty(p int){
	self.Property = p
}
func (self *Ai) GetBeginTimeDiff() int64 {
	return time.Now().Unix() - self.BeginTime
}
func (self *Ai) UpdateBeginTime(){
	self.BeginTime = time.Now().Unix()
}
func (self *Ai) SetLastUid(uid int) {
	self.LastPublicId = uid
}
func (self *Ai) InitNow(str []byte){
	self.Public[3].SetNow(str)
	for i,bl := range self.Blocks {
		if bl == nil {
			bl = new(NewBlock)
			bl.Init(self.Public[3].Now[i][:],i)
			self.Blocks[i] = bl
			self.BlocksMap[i] = bl
		}else{
			bl.Update()
		}
		SortNewBlock(self.Blocks[:],i)
	}
	if self.Public[3].Inv == -1 {
		self.Public[3].Inv = self.GetDiscard()
	}
	fmt.Println(self.Public[3].Now)
}
func (self *Ai) UpdateBlocksWeight(add bool,i,j,n int ){

	if add {
		self.Public[3].Now[i][j] += n
	}else {
		self.Public[3].Now[i][j] -= n
	}
	self.BlocksMap[i].Update()

}
func (self *Ai) SetSumVal(n int) {
	self.SumVal = n
}
func (self *Ai) SetBaseVal(n int) {
//	self.Public[nid].BaseVal = n
	self.BaseVal = n
}
func (self *Ai) GetFullVal( n int) (val int) {
//	fmt.Println("full val",n,self.BaseVal,self.Public[3].NowVal)
	if n ==0 || self.BaseVal == 0 || self.Public[3].NowVal == 0 {
		return 0
	}
	self.Public[3].TmpNowVal = n
	val =  (self.Public[3].NowVal-n )/self.BaseVal
	if val>100{
		val = 0
	}
	return val
}
func (self *Ai) SetLastSee(uid int,n int) {
	self.Public[uid].LastSee = n
	self.LastPublicId = uid
}
func (self *Ai) GetLastSee() int {
	if self.LastPublicId <0 || self.LastPublicId >2 {
		return -1
	}
	return self.Public[self.LastPublicId].LastSee
}
func (self *Ai) SetLastForData(n int,s []byte){
	self.LastForData[n] = s
}
func (self *Ai) GetLastForData() [2][]byte{
	return self.LastForData
}
func (self *Ai) GetNowVal(nid int) int{
	return self.Public[nid].NowVal
}
func (self *Ai) SetNowVal(nid int,n int){
//	fmt.Println("NowVal",nid,n)
	self.Public[nid].NowVal = n
//	self.NowVal = n
}
func (self *Ai) SetValid(nid int,n int){
	self.Public[nid].Inv = n
}
func (self *Ai) GetValid(nid int) int{
	return self.Public[nid].Inv
}
func (self *Ai) SetPublicDown(nid int,n int,val int){
	self.Public[nid].Down[n/9][n%9] = val
}
func (self *Ai) SetPublicSee(nid int,n int){
	fmt.Println("see",nid,n)
	self.Public[nid].See[n/9][n%9] ++
	self.SetLastSee(nid,n)
}
func (self *Ai) GetDiscard() int{

	if self.Blocks[0].SumLen != self.Blocks[1].SumLen {
		return self.Blocks[0].I
	}
	if self.Blocks[0].SumWei > self.Blocks[1].SumWei {
		return self.Blocks[0].I
	}else if self.Blocks[0].SumWei < self.Blocks[1].SumWei {
		return self.Blocks[1].I
	}
	if len(self.Blocks[0].Child) > len(self.Blocks[1].Child) {
		return self.Blocks[0].I
	}else if len(self.Blocks[0].Child) < len(self.Blocks[1].Child) {
		return self.Blocks[1].I
	}
	return self.Blocks[0].I

}
func (self *Ai) SelfSee(isf int,cOut int) (int) {
	if cOut == -1 {
		f := self.checkFull(true)
		if f>0 {
	//		if f >= isf {
			return 33
	//		}
		}
	}
	for i,no := range self.Public[3].Now {
		if i == self.Public[3].Inv {
			continue
		}
		for j,n := range no {
			if n == 0 {
				continue
			}
			if n == 4 {
		//		self.Public[3].Down[i][j] = 4
				return 32
			}
			_n := self.Public[3].Down[i][j]
			if _n == 0 {
				continue
			}
			if n > 1 {
				self.Public[3].Down[i][j] = 0
				continue
			}

			gr1 := self.CheckJiao(isf,true)
			if len(gr1) == 0 {
		//		self.Public[3].Down[i][j] = 3
				return 32
			}
			self.UpdateBlocksWeight(false,i,j,n)
			gr2 := self.CheckJiao(isf,true)
	//		self.UpdateBlocksWeight(true,i,j,n)
			if len(gr2) >0 {
		//		self.Public[3].Down[i][j] = 3
				return 32
			}
		}
	}
	return cOut

}
func (self *Ai) CheckAllJiao(isf int) (gr *MJGrain) {
	ji :=0
	var for4 []*MJGrain
	var jiao []*MJGrain
	kn:=2
	if isf >0 {
		kn =1
	}
	for _,bl := range self.Blocks {
		for _,_b := range bl.Child{
			for _,j := range _b.Bl {
			//	n :=bl.No[j]
			//	if  n>2 {
			//		continue
			//	}
				bl.No[j]--
				bl.Update()
				gs := self.CheckJiao(isf,true)
				bl.No[j]++
				lgs := len(gs)
				if lgs == 0 {
					continue
				}
				n1 :=0
				for _,g:= range gs {
					n1 += 4-self.GetPublicNum(g,true)
				}
				gr := &MJGrain{H:bl.I,N:j,O:[]int{0,0,0,0,0}}
				gr.O[0] = lgs
				gr.O[1] = n1
				gr.O[2] = 0
				if self.GetPublicNum(gr,false) >0 {
					gr.O[2] = 1
				}
				gr.O[3] =gs[0].O[0]
				gr.O[4] = bl.No[j]
				if bl.No[j] >kn {
					for4 = append(for4,gr)
				}else{
					jiao = append(jiao,gr)
				}
				ji++
			}
		}
		bl.Update()
	}
	if len(jiao)>0{
		return SortGrainArrs(jiao,0,true,true,true,true,false)
	}else if len(for4)>0 {
		if isf <= 0 {
			return SortGrainArrs(for4,0,true,true,true,true,false)
		}
	}
	return nil
	//return jiao
}
func (self *Ai) EasyOuts() (gr *MJGrain) {
	var grs []*MJGrain
	for _,bl := range self.Blocks {
		grs = append(grs,bl.GetMJGrainList(self)...)
	}
	gr = SortGrainArrs(grs,0,false,true,false,false)
	return gr
}
func (self *Ai) Outs(isf int,No30 bool) (out int){
	out = -1
	InvBl := self.BlocksMap[self.GetValid(3)]
	var gr *MJGrain = nil
//	defer func(){
//		if gr != nil {
//		//	self.Public[3].See[gr.H][gr.N]++
//		//	self.Public[3].Now[gr.H][gr.N]--
//		//	fmt.Println("self See",self.Public[3].Now,isf)
//		}
//	}()
	if InvBl.SumLen >0 {
		G:
		for _,bl := range InvBl.Child {
			for _,j := range bl.Bl {
				gr = &MJGrain{H:InvBl.I,N:j}
				out =  InvBl.I *9 + j
				break G
			}
		}
	}
	if No30 {
		out = self.SelfSee(isf,out)
	}
	if out > -1 {
		fmt.Println("out -1")
		return out
	}
	// jiao
	gr = self.CheckAllJiao(isf)
	if gr != nil {
		//gr = SortGrainArrs(jiao,0,true,true,true,true,false)
		fmt.Println("out 1")
		return gr.H*9+gr.N
	}
	if isf>0 {
		Gs:
		for _,bl := range self.Blocks{
			if bl == InvBl {
				continue Gs
			}
			if bl.SumLen < 3 {
				for _,n:= range self.Public[3].Down[bl.I]{
					if n>0{
						break Gs
					}
				}
				for _,ch := range bl.Child{
					for _,j := range ch.Bl{
						fmt.Println("out 1 f")
						return bl.I*9+j
					}
				}
			}
		}
	}
	var grs []*MJGrain = nil
	for _,bl := range self.Blocks {
		if bl.SumLen ==0 {
			continue
		}
		grs = append(grs,bl.RunFind(self)...)
	}
	if len(grs) > 0 {
		gr = SortGrainArrs(grs,0,false,false,false,true)
	//	str := ""
	//	for _,g := range grs {
	//		str = fmt.Sprintf("%s %d",str,g.H*9+g.N)
	//	}
	//	fmt.Println("out 2",str)
		fmt.Println("out 2")
		if (isf>0) {
			if self.Public[3].Now[gr.H][gr.N]>1 {
				fmt.Println("out 3")
				gr =self.EasyOuts()
			}
		}
		return gr.H*9 +gr.N
	}
	gr =self.EasyOuts()
	fmt.Println("out 3")
	return gr.H*9 +gr.N


}
func (self *Ai) ReloadSelfInv() {
	self.Public[3].Inv = self.GetDiscard()
}
func (self *Ai) OutDiscard(num int) (o []byte){

	if self.Public[3].Inv == -1 {
		self.Public[3].Inv = self.GetDiscard()
	}
	bl := self.BlocksMap[self.Public[3].Inv]
	G:
	for _,bs := range bl.Child{
		for _,j:= range bs.Bl {
			n := self.Public[3].Now[bl.I][j]
			for i:=0;i<n;i++{
				self.Public[3].Now[bl.I][j]--
				o = append(o,byte(bl.I*9 + j))
				num --
				if num == 0 {
					break G
				}
			}
		}
	}
	bl.Update()
	for i :=num; i>0; i-- {
		gr:=self.EasyOuts()
	//	out := self.Outs(-1,false)
		self.Public[3].Now[gr.H][gr.N]--
		o = append(o,byte(gr.H*9+gr.N))
//		self.BlocksMap[]
	}
	return o

}
//func (self *Ai) SeeOut(v int,isf int) int{
//	return 0
//}

func (self *Ai) SeeOuts(v int,key []byte,isf int) byte {

	if v <0 || v > 26 {
		return 0
	}
	i:=v/9
	j := v%9
//	ns :=0
//	no :=self.Public[3].Now[i][j]

//	defer func(){
//		fmt.Println("Down",v,isf)
//		self.Public[3].Now[i][j] = no
//		self.BlocksMap[i].Update()
//	//	self.Public[3].Down[i][j] = ns
//	}()
	var ks [4]byte = [4]byte{5,5,5,5}
	for _,k := range key {
		if k >3 {
			continue
		}else{
			ks[3-k] = k
		}
	}
	for _,k := range ks {
		if k == 3 {
		//	return k
			self.Public[3].Now[i][j]++
			self.BlocksMap[i].Update()
			f := self.checkFull(false)
			if f == 0 {
				return k
			}
			if f>0 && f >= isf {
				return k
			}
			continue
		}else if k == 2 {
		//	ns = 3
			return k

		//	gr1 := self.CheckJiao(isf,true)
		//	if len(gr1) == 0 {
		//		ns = 3
		//		return k
		//	}
		//	self.Public[3].Now[i][j]=0
		//	self.BlocksMap[i].Update()
		//	gr2 := self.CheckJiao(isf,true)
		//	if len(gr2)>0 {
		//		ns = 3
		//		return k
		//	}
		//	continue
		}else if k == 1 {
			ns := self.Public[3].Now[i][j]
			if ns <2 {
				return k
			}
			gr1 := self.CheckJiao(isf,true)
			if len(gr1) == 0 {
			//	ns = 2
				return k
			}
			self.Public[3].Now[i][j]=0
			self.BlocksMap[i].Update()
			if self.CheckAllJiao(isf) != nil {
			//	ns = 2
				return k
			}

		//	for gi:=0;gi <lg2;gi++{
		//		SortdGrain(gr2,gi,0)
		//	}
		//	if gr2[0].O[0] >= len(gr1) {
		//		ns = 2
		//		return k
		//	}
		//	for gi:=0;gi < lg2;gi++{
		//		SortdGrain(gr2,gi,1)
		//	}
		//	sum1 :=0
		//	for _,g := range gr1 {
		//		sum1 += 4 - self.GetPublicNum(g,true)
		//	}
		//	if gr2[0].O[1] >= sum1 {
		//		ns = 2
		//		return k
		//	}
		//	for gi:=0;gi < lg2;gi++{
		//		SortdGrain(gr2,gi,3)
		//	}
		//	if gr2[0].O[3] >= gr1[0].O[0] {
		//		ns = 2
		//		return k
		//	}
		//	continue
		}else if k == 0 {
			return 0
		}
	}
	return 0

}
func (self *Ai) CheckJiao(isf int,isM bool) (gr []*MJGrain) {

	I :=0
	for _,bl := range self.Blocks{
		if bl.I == self.Public[3].Inv {
			if bl.SumLen > 0 {
				return nil
			}
			continue
		}
		for i,n := range bl.No {
			if n == 4 {
				continue
			}
			bl.No[i]++
			bl.Update()
		//	self.updateBlocksWeight(true,bl.I,i)
			fn := self.checkFull(isM)

			if fn > 0 && fn >= isf {
				g :=&MJGrain{H:bl.I,N:i,O:[]int{fn}}
			//	ns :=self.GetPublicNum(g,true)
			//	if ns == 4 {
			//		continue
			//	}
			//	g.O = append(g.O,ns)
				gr = append(gr,g)
				SortdGrain(gr,I,0)
				I ++
			}
			bl.No[i]--

		}
		bl.Update()
	}
	return gr

}
func (self *Ai) GetPublicNum(gr *MJGrain,isNow bool) (n int) {

	if isNow {
		n = self.Public[3].Now[gr.H][gr.N]

	}else{
		n = 0
	}
	for _,pu := range self.Public {
		n += pu.Down[gr.H][gr.N]
		n += pu.See[gr.H][gr.N]
	}
	return n

}
func (self *Ai) checkFull(Max bool) int {
	w :=0
	dui := 0
	dan := 0
	qing:= 0
	f := 1
	for _,bl := range self.Blocks {
//		bl.Init(self.Public[3].Now[bl.I][0:],bl.I)
		if bl.I == self.Public[3].Inv && bl.SumLen > 0 {
			return 0
		}
		w += bl.W
		for _,b := range bl.Child {
			for _,j := range b.Bl {
				n :=bl.No[j]
				if n == 2 {
					dui ++
				}else if n == 4 {
					dui += 2
					f += 1
				}else if n == 1 {
					dan ++
				}
			}
		}
		if bl.SumLen >0 {
			qing++
		}else{
			for _,n := range self.Public[3].Down[bl.I] {
				if n >0 {
					qing++
					break
				}
			}
		}
	}
	if w != 1 && dui != 7 {
		return 0
	}
	if qing == 1 || dui ==7 {
		f = 4
	}
	if dan == 0 && dui ==1 {
		f *= 2
	}
	for i,no := range self.Public[3].Down {
		for j,n := range no {
			if n >2 {
				f ++
			}else if n == 2 {
				if self.Public[3].Now[i][j] >0 {
					f ++
				}
			}
		}
	}
	if Max {
	//	old := f
		f++
		for _,p := range self.Public[:3]{
			if p.IsF {
				f += f
			}
		}
	//	if f == old {
	//		f*=2
	//	}
	}
	return f

}
