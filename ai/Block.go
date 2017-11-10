package ai
import(
//	"strconv"
)
type Block struct {
	Bl [][]int
	Num int
	Numi int
	No []int
	I  int
	W  int
	We []*MJGrain
}
//func (self *Block) FindWeight(grs []*MJGrain) {
func (self *Block) RunFind(a *Ai ) []*MJGrain {
	if len(self.We) == 0 {
		return nil
	}
	var grs []*MJGrain
	for _,g := range self.We {
	//	if self.No[i] >2 {
	//		continue
	//	}
	//	g.O[0] += self.No[g.N]
		g.O = append(g.O,2)
		g.O = append(g.O,a.GetPublicNum(g,false))
		g.O = append(g.O,self.No[g.N])
		g.O = append(g.O,0)
		grs = append(grs,g)
	}
	var No [9]int
	copy(No[0:],self.No)
	for i,n := range No {
		if n == 4 {
			continue
		}
		if n == 0 {
			isj := false
			i_ := i+1
			if i_<9 {
				if No[i_] > 0 {
					isj = true
				}
			}
			if !isj {
				i_  = i-1
				if i_ >= 0 {
					if No[i_] > 0 {
						isj = true
					}

				}
			}
			if !isj {
				continue
			}
		}
		_n := a.GetPublicNum(&MJGrain{H:self.I,N:i},true)
		if _n == 4 {
			continue
		}
		_n = 4 - _n
		No[i] ++
		tmpBlock := new(Block)
		tmpBlock.Init(No[0:],self.I)
	//	if len(self.We) >= len(tmpBlock.We) {
			tmpBlock.Find(a,grs,0,_n*10)
	//	}
		No[i] --
	}
	return grs
}
func  (self *Block)Find(a *Ai,grs []*MJGrain,level int,num int) {
//	if len(self.We) == 0 {
//		return
//	}
	var NewGrs []*MJGrain
	for _,g := range grs {
		isH := false
		for _,gn := range self.We {
			if gn.N == g.N {
				isH = true
				NewGrs = append(NewGrs,g)
				break
			}
		}
		if !isH {
			if g.O[1] > level  {
				g.O[1] = level
				g.O[4] = num
			}else if g.O[1] == level {
				g.O[4] += num
			}
		}
	}
	if len(NewGrs) ==  0 {
		return
	}
	level ++
	if level > 1 {
		return
	}
	var No [9]int
	copy(No[0:],self.No)
	for i,n := range No {
		if n == 4 {
			continue
		}
		if n == 0 {
			isj := false
			i_ := i+1
			if i_<9 {
				if No[i_] > 0 {
					isj = true
				}
			}
			if !isj {
				i_  = i-1
				if i_ >= 0 {
					if No[i_] > 0 {
						isj = true
					}

				}
			}
			if !isj {
				continue
			}
		}
		_n := a.GetPublicNum(&MJGrain{H:self.I,N:i},true)
		if _n == 4 {
			continue
		}
		_n = 4 - _n
		No[i] ++
		tmpBlock := new(Block)
		tmpBlock.Init(No[0:],self.I)
	//	if len(self.We) >= len(tmpBlock.We) {
			tmpBlock.Find(a,NewGrs,level,_n+num)
	//	}
		No[i] --
	}

}
func (self *Block) Update() {
	self.Num = 0
	self.Numi = 0
	self.Bl = nil
	self.We = nil

	var tmparr []int = nil

	for j,n := range self.No {
		if n == 0 {
			if (len(tmparr)>0){
				self.Bl = append(self.Bl,tmparr)
				tmparr = nil
			}
			continue
		}
		self.Num += n
		self.Numi ++
		tmparr = append(tmparr,j)
	}
	if (len(tmparr)>0){
		self.Bl = append(self.Bl,tmparr)
		tmparr = nil
	}

	self.SetWeight()
}
func (self *Block) Init(no []int,i int) {

	self.I = i
	self.No = no

	self.Num = 0
	self.Numi = 0
	self.Bl = nil
	self.We = nil

	var tmparr []int = nil

	for j,n := range no {
		if n == 0 {
			if (len(tmparr)>0){
				self.Bl = append(self.Bl,tmparr)
				tmparr = nil
			}
			continue
		}
		self.Num += n
		self.Numi ++
		tmparr = append(tmparr,j)
	}
	if (len(tmparr)>0){
		self.Bl = append(self.Bl,tmparr)
		tmparr = nil
	}

	self.SetWeight()
}

func (self *Block) GetWeight() int {

	return self.W

}

func (self *Block) SetWeight() {

	self.W = 0
	self.We = nil
	var No [9]int
	copy(No[0:],self.No)

	for _,bl := range self.Bl {
		w,e := GetWeightVal(No,bl)
		self.W += w
		for _,_e := range e {
			_e.H = self.I
			self.We = append(self.We,_e)
		}
	}

}
func WeightVal(No [9]int,bl []int,w int,o []*MJGrain) (int,[]*MJGrain) {
	le := len(bl)
	if le == 0 {
		return w,o
	}
	if le < 3 {
		for _,i := range bl {
			if No[i] < 3 {
				v:=(3 - No[i])
				w += v
				//if No[i] == 1 {
				if No[i]>0{
					o = append(o,&MJGrain{H:-1,N:i,O:[]int{No[i]}})
				}
			}
		}
		return w,o
	}
	n:= No[bl[0]]
	if n > 2 {
		return WeightVal(No,bl[1:],w,o)
	}
	var start int = 3
	isb := false
	for i:=2;i>=0;i--{
		No[bl[i]] --
		if No[bl[i]] == 0 {
			if !isb {
				isb = true
			}
			continue
		}else {
			if !isb {
				start = i
			}else{
				v :=3 - No[bl[i]]
				w += v
				if No[i]>0{
					o = append(o,&MJGrain{H:-1,N:i,O:[]int{No[i]}})
				}
			}
		}
	}

	bl = bl[start:]
	len_bl := len(bl)
	if len_bl < 3 {
		return WeightVal(No,bl,w,o)
	}

	w1,o1:= WeightVal(No,bl,0,nil)
	rbl := make([]int,len_bl)
	for i,n := range bl {
		rbl[len_bl - i - 1] = n
	}
	w2,o2 := WeightVal(No,rbl,0,nil)

	if w2 < w1 {
		return w2+w,append(o,o2...)
	}else if w2 > w1 {
		return w1+w,append(o,o1...)
	}
	for _,_o1 := range o1 {
		isS := false
		for _,_o2 := range o2 {
			if _o1.N == _o2.N {
				_o2.O[0]--
				isS = true
				break
			}
		}
		if !isS {
			o = append(o,_o1)
		}
	}
	o = append(o,o2...)
	return w1+w,o
}
func GetWeightVal(No [9]int, bl []int) (int,[]*MJGrain) {
//	var o1 []*MJGrain = nil
//	_w1,o1 := WeightVal(No,bl,0,nil)
//	if  _w1 == 0 {
//		return 0,o1
//	}
	l := len(bl)
	if l < 3 {
		return WeightVal(No,bl,0,nil)
	}
	_w1,o1:= WeightVal(No,bl,0,nil)
	rs := make([]int,l)
	for i,_s := range bl {
		rs[l-i-1] = _s
	}
	_w2,o2 := WeightVal(No,rs,0,nil)
	if _w2 < _w1 {
		return _w2,o2
	}else if _w2 > _w1 {
		return _w1,o1
	}

	var no []*MJGrain = nil
	for _,_o := range o2 {
		isS := false
		for _,o_ := range o1 {
			if _o.N == o_.N {
				o_.O[0]--
				isS = true
				break
			}
		}
		if !isS {
			no = append(no,_o)
		}
	}
	o1 = append(o1,no...)

	return _w1,o1

}
func SortBlock(bs []*Block,i int) {
	if i == 0 {
		return
	}
	I := i-1
	if bs[i].Num < bs[I].Num {
		bs[i],bs[I] = bs[I],bs[i]
	}
	SortBlock(bs,I)
}
