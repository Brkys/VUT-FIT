Zden�k Brhel - xbrhel04

Konvence spou�t�n�
./ipk-lookup [-h] � vyp�e n�pov�du
./ipk-lookup -s server [-T timeout] [-t type] [-i] name � spust� program
�	h (help) - voliteln� parametr, p�i jeho zad�n� se vyp�e n�pov�da a program se ukon��.
�	s (server) - povinn� parametr, DNS server (IPv4 adresa), na kter� se budou odes�lat dotazy.
�	T (timeout) - voliteln� parametr, timeout (v sekund�ch) pro dotaz, v�choz� hodnota 5 sekund.
�	t (type) - voliteln� parametr, typ dotazovan�ho z�znamu: A (v�choz�), AAAA, NS, PTR, CNAME.
�	i (iterative) - voliteln� parametr, vynucen� iterativn�ho zp�sobu rezoluce, viz d�le.
�	name - p�ekl�dan� dom�nov� jm�no, v p��pad� parametru -t PTR program na vstupu naopak o�ek�v� IPv4 nebo IPv6 adresu.
