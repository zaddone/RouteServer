package scann
import(
//	"net/http"
//	"fmt"
	"database/sql"
)
const(
//	scannIPdb = "/db/ProxyWeb.db"
)

func CreateProxyWeb() error {
	db,err := sql.Open("sqlite3",scannPortdb)
	if err != nil {
		return err
	}
	
	_,err = db.Exec("CREATE TABLE `scannPort` (`port` INTEGER PRIMARY KEY NOT NULL,`order` INTEGER)")
	return err
}
