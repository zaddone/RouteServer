package mjai
import(
//	""
)
type ChildBlock struct {
	Bl []int
	Wei int
	Len int
}
type NewBlock struct {
	No []int
	I  int
	Child []*ChildBlock
	We []*MJGrain
	SumWei int
	SumLen int
	W int
}
func (self * NewBlock) RunFind(a *Ai) []*MJGrain {

	for _,g := range self.We {
	//	var Os []int = nil
	//	Os = append(Os,g.O[0],a.GetPublicNum(g,false),g.O[1],g.O[2])
		Os := []int{g.O[0],a.GetPublicNum(g,false),g.O[1],g.O[2]}
		g.O = Os
	}
	return self.We
//	self.We = append(self.We,_e)
}
func (self * NewBlock) Init(no []int,i int){
	self.No = no
	self.I = i
	self.Update()
}
func (self * NewBlock) GetMJGrainList(a *Ai) (gs []*MJGrain) {
	for _,ch:= range self.Child {
		for _,j := range ch.Bl {
			g:=&MJGrain{H:self.I,N:j,O:nil}
			g.O = []int{self.No[j],a.GetPublicNum(g,false),ch.Wei,ch.Len}
			gs = append(gs,g)
		}
	}
	return gs
}
func (self * NewBlock) Update(){
	self.Child = nil
	self.SumLen = 0
	self.SumWei = 0
	self.We = nil

	child := new(ChildBlock)
	for j,n := range self.No {
		if n == 0 {
			if (child.Wei>0){
				self.Child = append(self.Child,child)
				self.SumLen += child.Len
				self.SumWei += child.Wei
				child = new(ChildBlock)
			}
			continue
		}
		child.Bl =append(child.Bl,j)
		child.Len += n
		child.Wei ++
	}
	if ( child.Wei > 0 ){
		self.Child = append(self.Child,child)
		self.SumLen += child.Len
		self.SumWei += child.Wei
	}

	self.SetWeight()
}
func (self *NewBlock) SetWeight() {

	self.W = 0
	self.We = nil
	var No [9]int
	copy(No[0:],self.No)
	for _,ch := range self.Child {
		w,e := GetWeightVal(No,ch.Bl)
		self.W += w
		for _,_e := range e {
			_e.H = self.I
			_e.O = append(_e.O,ch.Wei,ch.Len)
			self.We = append(self.We,_e)
		}
	}

}
func SortNewBlock(bs []*NewBlock,i int) {
	if i == 0 {
		return
	}
	I := i-1
	if bs[i].SumLen < bs[I].SumLen {
		bs[i],bs[I] = bs[I],bs[i]
	}
	SortNewBlock(bs,I)
}
