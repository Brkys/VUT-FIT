Zdeněk Brhel xbrhel04

Konvence zapisování parametrů u klienta
./ipk-client -h {host} -p {port} [-l|-n|-f] {login}
./ipk-client – Jméno aplikace
-h {host} – Povinný parametr, host reprezentuje IP adresu serveru, nebo jeho DNS jméno
-p {port} –  Povinný parametr, port reprezentuje číslo cílového portu
[-l|-n|-f] {login} – Je třeba vybrat si z trojice parametrů. V případě -l vypíše všechny jména začínající řetězcem login, pokud není řetězec zadán, vypíše všechny. V případě -n vypíše informace o uživateli, řetězec login je povinný parametr a nenalezne-li server takového uživatele, vypíše do konzole user not found. V případě -f vypíše cestu k uživatelskému domovskému adresáři.

Konvence zapisování parametrů u klienta
./ipk-server -p {port}
./ipk-client – Jméno aplikace
-h {host} – Povinný parametr, host reprezentuje IP adresu serveru, nebo jeho DNS jméno
-p {port} –  Povinný parametr, port reprezentuje číslo portu, na kterém server naslouchá

Odesílání výstupu ze serveru má omezené místo
