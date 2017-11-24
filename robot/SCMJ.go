package robot
import(
	"fmt"
	"../comm"
)
type SCMJ interface{
	Init()
	InitNow(str []byte)
	SetSeeList(b []byte)
	ReloadSelfInv()
	SetNowVal(nid int,n int)
	GetNowVal(nid int) int
	UpdateBlocksWeight(add bool,i,j,n int )
//	Begin1(b []byte) []byte

	SetLastSee(nid int,n int)
	GetLastSee() int
	Outs(isf int,No30 bool) int
	OutDiscard(num int) (o []byte)
	GetDiscard() int
	SeeOuts(v int,key []byte,isf int) byte
//	SetLastForData(n int,s []byte)
//	GetLastForData() [2][]byte
	SetSumVal(n int)
	SetValid(nid int,n int)
	GetValid(nid int) int
	SetPublicDown(nid int,n int,val int)
	SetPublicSee(nid int,n int)
	GetFullVal(n int) int
	SetBaseVal(n int)
}
var (
	ScmjFuncLib map[uint16]HandlefuncType
	ScmjId byte = 'O'
)
func LoadScmjFuncLib(){
	ScmjFuncLib = make(map[uint16]HandlefuncType)
	ScmjFuncLib[11]=func(b []byte,u *UserBase) []byte{
		fmt.Println("func 11",b)
//		u.Scmj.SetSeeList(b)
		return nil
	}
	ScmjFuncLib[10]=func(b []byte,u *UserBase) []byte{
		fmt.Println("func 10",b)
		u.Scmj.SetSeeList(b)
		return nil
	}
	ScmjFuncLib[9]=func(b []byte,u *UserBase) []byte{
		u.Scmj.SetNowVal(int(b[0]),comm.ByteToInt(b[1:]))
		return nil
	}
	ScmjFuncLib[6]=func(b []byte,u *UserBase) []byte{
		u.Scmj.InitNow(b[:14])
		n :=u.Scmj.Outs(u.Scmj.GetFullVal(comm.ByteToInt(b[14:])),false)
		fmt.Println("func 4",b[:14],n)
		return []byte{byte(n)}
	}
	ScmjFuncLib[5]=func(b []byte,u *UserBase) []byte{
		fmt.Println("func 5",b[:4])
		val := u.Scmj.SeeOuts(u.Scmj.GetLastSee(),b[:4],u.Scmj.GetFullVal(comm.ByteToInt(b[4:])))
		return []byte{byte(val)}
	}
	ScmjFuncLib[4]=func(b []byte,u *UserBase) []byte{
		u.Scmj.InitNow(b[:14])
		n :=u.Scmj.Outs(u.Scmj.GetFullVal(comm.ByteToInt(b[14:])),true)
		fmt.Println("func 4",b[:14],n)
		return []byte{byte(n)}
	}
	ScmjFuncLib[3]=func(b []byte,u *UserBase) []byte{
		for _,_b := range b[:14]{
			if _b<27{
				u.Scmj.SetPublicDown(3,int(_b),1)
			}
		}
		u.Scmj.InitNow(b[14:28])
		n :=u.Scmj.Outs(u.Scmj.GetFullVal(comm.ByteToInt(b[28:])),true)
		fmt.Println("func 3",b[:14],b[14:28],n)
		return []byte{byte(n)}
	}
	ScmjFuncLib[2]=func(b []byte,u *UserBase) []byte{
		fmt.Println("func 2",b[:14])
		u.Scmj.Init()
		u.Scmj.InitNow(b[:14])
		u.Scmj.SetBaseVal(int(b[14]))
	//	u.Scmj.ReloadSelfInv()
		a := u.Scmj.GetValid(3)
//		n := u.Scmj.GetNowVal(3)
		return []byte{byte(a)}
	//	if n>0 {
	//		return []byte{byte(a)}
	//	}else{
	//		return []byte{byte(a),1}
	//	}
	}
	ScmjFuncLib[1]=func(b []byte,u *UserBase) []byte{
		u.Scmj.Init()
		fmt.Println("func 1",b[:14])
		u.Scmj.InitNow(b[:14])
		u.Scmj.SetNowVal(3,comm.ByteToInt(b[14:]))
		return u.Scmj.OutDiscard(3)
//		return u.Scmj.Begin1(b)
	}
}
