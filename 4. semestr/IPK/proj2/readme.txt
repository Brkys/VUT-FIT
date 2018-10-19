Zdenìk Brhel - xbrhel04

Konvence spouštìní
./ipk-lookup [-h] – vypíše nápovìdu
./ipk-lookup -s server [-T timeout] [-t type] [-i] name – spustí program
•	h (help) - volitelný parametr, pøi jeho zadání se vypíše nápovìda a program se ukonèí.
•	s (server) - povinný parametr, DNS server (IPv4 adresa), na který se budou odesílat dotazy.
•	T (timeout) - volitelný parametr, timeout (v sekundách) pro dotaz, výchozí hodnota 5 sekund.
•	t (type) - volitelný parametr, typ dotazovaného záznamu: A (výchozí), AAAA, NS, PTR, CNAME.
•	i (iterative) - volitelný parametr, vynucení iterativního zpùsobu rezoluce, viz dále.
•	name - pøekládané doménové jméno, v pøípadì parametru -t PTR program na vstupu naopak oèekává IPv4 nebo IPv6 adresu.
