------------------------------------
--             MAZANI             --
------------------------------------
DROP PROCEDURE printMurderers;
DROP PROCEDURE percentMurder;
DROP SEQUENCE lastId;
DROP INDEX myIndex;
DROP TABLE Familie CASCADE CONSTRAINTS;
DROP TABLE Aliance CASCADE CONSTRAINTS;
DROP TABLE Clen CASCADE CONSTRAINTS;
DROP TABLE Kriminalni_cinnost CASCADE CONSTRAINTS;
DROP TABLE FPozvanyS CASCADE CONSTRAINTS ;
DROP TABLE Setkani_donu CASCADE CONSTRAINTS ;
DROP TABLE Uzemi CASCADE CONSTRAINTS ;
DROP TABLE Vrazda CASCADE CONSTRAINTS ;
DROP TABLE Caporegime CASCADE CONSTRAINTS ;
DROP TABLE Radovy_clen CASCADE CONSTRAINTS ;
DROP TABLE FVedeK CASCADE CONSTRAINTS ;
DROP TABLE CProvadiK CASCADE CONSTRAINTS ;
DROP TABLE FOperujeU CASCADE CONSTRAINTS ;
DROP MATERIALIZED VIEW pohled;

------------------------------------
--            TABULKY             --
------------------------------------

CREATE TABLE Familie (
  ID_Familie NUMBER NOT NULL PRIMARY KEY,
  JmenoFamilie VARCHAR(30) NOT NULL,
  JmenoDona VARCHAR(30) NOT NULL,
  DatumNarozeniDona DATE NOT NULL,
  VelikostBotDona NUMBER NOT NULL,
  PohlaviDona CHAR NOT NULL,
  BarvaVlasuDona VARCHAR(6),
  JmenoConsigliere VARCHAR(30) NOT NULL
);

CREATE TABLE Aliance(
  ID_Aliance NUMBER NOT NULL PRIMARY KEY,
  Aliance1 NUMBER NOT NULL,
  Aliance2 NUMBER NOT NULL,
  DatumZacatku DATE NOT NULL ,
  DatumUkonceni DATE NOT NULL,
  CONSTRAINT Tvori_alianci1 FOREIGN KEY (Aliance1) REFERENCES Familie(ID_Familie) ON DELETE CASCADE ,
  CONSTRAINT Tvori_alianci2 FOREIGN KEY (Aliance2) REFERENCES Familie(ID_Familie) ON DELETE CASCADE
);

CREATE TABLE Uzemi(
  ID_Uzemi NUMBER NOT NULL PRIMARY KEY,
  Majitel NUMBER,
  Adresa VARCHAR(50) NOT NULL,
  Rozloha NUMBER NOT NULL,
  CONSTRAINT Vlastni FOREIGN KEY (Majitel) REFERENCES Familie(ID_Familie) ON DELETE CASCADE
);

CREATE TABLE Kriminalni_cinnost(
  ID_Cinnosti NUMBER NOT NULL PRIMARY KEY,
  ID_Uzemi NUMBER NOT NULL,
  TypCinnosti VARCHAR(30) NOT NULL,
  DatumZacatku DATE NOT NULL,
  DatumUkonceni DATE NOT NULL,
  CONSTRAINT ProbihaNa FOREIGN KEY (ID_Uzemi) REFERENCES Uzemi(ID_Uzemi) ON DELETE CASCADE
);

CREATE TABLE FVedeK(
  ID_Familie NUMBER NOT NULL,
  ID_Cinnosti NUMBER NOT NULL,
  CONSTRAINT Vede PRIMARY KEY (ID_Familie, ID_Cinnosti),
  CONSTRAINT FK_KFamilie FOREIGN KEY (ID_Familie) REFERENCES Familie(ID_Familie) ON DELETE CASCADE ,
  CONSTRAINT FK_KCinnosti FOREIGN KEY (ID_Cinnosti) REFERENCES Kriminalni_cinnost(ID_Cinnosti) ON DELETE CASCADE
);

CREATE TABLE Vrazda(
  ID_Cinnosti NUMBER NOT NULL,
  Obet VARCHAR(30) NOT NULL,
  CONSTRAINT ID_Vrazdy PRIMARY KEY (ID_Cinnosti, Obet),
  CONSTRAINT FK_Vrazda FOREIGN KEY (ID_Cinnosti) REFERENCES Kriminalni_cinnost(ID_Cinnosti) ON DELETE CASCADE
);

CREATE TABLE Setkani_donu(
  ID_Setkani NUMBER NOT NULL PRIMARY KEY,
  ID_Uzemi NUMBER NOT NULL,
  DatumSetkani DATE NOT NULL,
  CONSTRAINT Probiha FOREIGN KEY (ID_Uzemi) REFERENCES Uzemi(ID_Uzemi) ON DELETE CASCADE
);

CREATE TABLE Clen(
  RC_Clena VARCHAR(11) NOT NULL PRIMARY KEY,
  ID_Familie NUMBER NOT NULL,
  Jmeno VARCHAR(30) NOT NULL,
  DatumNarozeni DATE NOT NULL,
  Pohlavi CHAR NOT NULL,
  CONSTRAINT Patri FOREIGN KEY (ID_Familie) REFERENCES Familie(ID_Familie) ON DELETE CASCADE
);

CREATE TABLE FPozvanyS(
  ID_Familie NUMBER NOT NULL,
  ID_Setkani NUMBER NOT NULL,
  Potvrdil NUMBER(1,0) NOT NULL,
  CONSTRAINT Setkani FOREIGN KEY (ID_Setkani) REFERENCES Setkani_donu(ID_Setkani) ON DELETE CASCADE,
  CONSTRAINT DonPotvrdil FOREIGN KEY (ID_Familie) REFERENCES  Familie(ID_Familie) ON DELETE CASCADE
);

CREATE TABLE Caporegime(
  RC_Clena VARCHAR(11) PRIMARY KEY NOT NULL REFERENCES Clen(RC_Clena) ON DELETE CASCADE,
  UrovenDuvery NUMERIC NOT NULL
);

CREATE TABLE Radovy_clen(
  RC_Clena VARCHAR(11) PRIMARY KEY NOT NULL REFERENCES Clen(RC_Clena) ON DELETE CASCADE,
  Oblibena_zbran VARCHAR(10) NOT NULL
);

CREATE TABLE CProvadiK(
  RC_Clena VARCHAR(11) NOT NULL,
  ID_Cinnosti NUMBER NOT NULL,
  CONSTRAINT Provadi PRIMARY KEY (ID_Cinnosti, RC_Clena),
  CONSTRAINT FK_Clena FOREIGN KEY (RC_Clena) REFERENCES Clen(RC_Clena) ON DELETE CASCADE ,
  CONSTRAINT FK_Cinnosti FOREIGN KEY (ID_Cinnosti) REFERENCES Kriminalni_cinnost(ID_Cinnosti) ON DELETE CASCADE
);

CREATE TABLE FOperujeU (
  ID_Familie NUMBER NOT NULL,
  ID_Uzemi NUMBER NOT NULL,
  CONSTRAINT Operuje PRIMARY KEY (ID_Familie, ID_Uzemi),
  CONSTRAINT FK_Familie FOREIGN KEY (ID_Familie) REFERENCES Familie(ID_Familie) ON DELETE CASCADE,
  CONSTRAINT FK_Uzemi FOREIGN KEY (ID_Uzemi) REFERENCES Uzemi(ID_Uzemi) ON DELETE CASCADE
);

------------------------------------
--            Triggery            --
------------------------------------
-- Kontroluje, zda ID clena je spravne rodne cislo
CREATE OR REPLACE TRIGGER RodCislo
  BEFORE INSERT OR UPDATE OF RC_Clena ON Clen
  FOR EACH ROW
DECLARE
  cislo Clen.RC_Clena%TYPE;
  rok VARCHAR(2);
  mesic VARCHAR(2);
  den VARCHAR(2);
  koncovka VARCHAR(4);
  kontrolni1 NUMBER;
  kontrolni2 NUMBER;

BEGIN
  cislo := :NEW.RC_Clena;
  rok := SUBSTR(cislo, 1, 2);
  mesic := SUBSTR(cislo, 3, 2);
  den := SUBSTR(cislo, 5, 2);
  koncovka := SUBSTR(cislo, 8, 4);
  kontrolni1 := TO_NUMBER(SUBSTR(rok, 1, 1)) + TO_NUMBER(SUBSTR(mesic, 1, 1)) + TO_NUMBER(SUBSTR(den, 1, 1)) + TO_NUMBER(SUBSTR(koncovka, 1, 1)) + TO_NUMBER(SUBSTR(koncovka, 3, 1));
  kontrolni2 := TO_NUMBER(SUBSTR(rok, 2, 1)) + TO_NUMBER(SUBSTR(mesic, 2, 1)) + TO_NUMBER(SUBSTR(den, 2, 1)) + TO_NUMBER(SUBSTR(koncovka, 2, 1)) + TO_NUMBER(SUBSTR(koncovka, 4, 1));
  IF(REGEXP_LIKE(cislo, '^[0-9]{6}/[0-9]{3,4}$')) THEN -- 001122/3344 nebo 001122/334
    IF(TO_NUMBER(mesic) > 70) OR (TO_NUMBER(mesic) < 1) THEN
      RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
    ELSIF(TO_NUMBER(den) < 1) OR (TO_NUMBER(den) > 31) THEN
      RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
    END IF;
  ELSE
    RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
  END IF;

  IF(LENGTH(cislo) = 10) THEN
    IF(TO_NUMBER(rok) > 53) THEN       --do roku 1953 vcetne mela RC jen devet cislic 123456/789
      RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
    END IF;
  ELSIF(LENGTH(cislo) = 11) THEN       --musi byt delitelne 11
    IF(MOD(kontrolni1-kontrolni2, 11) != 0) THEN
      RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
    END IF;
  END IF;
  EXCEPTION
    WHEN VALUE_ERROR THEN --v pripade nejakeho nesmyslu, zachytava chybu, ktera nastane pri konverzi TO_NUMBER
      RAISE_APPLICATION_ERROR(-20001, 'Spatny format rodneho cisla!');
END RodCislo;


CREATE SEQUENCE lastId
  INCREMENT BY 1;
CREATE OR REPLACE TRIGGER autoId
 BEFORE INSERT ON Familie
FOR EACH ROW
WHEN (new.ID_Familie IS NULL)
  BEGIN
    SELECT lastId.nextval
    INTO :new.ID_Familie
    FROM dual;
  end;
/

------------------------------------
--            Procedury           --
------------------------------------
--set serveroutput on;
CREATE OR REPLACE PROCEDURE percentMurder(idUZEMI IN NUMBER)
  IS
    CURSOR OBSAH IS SELECT ID_Cinnosti, ID_Uzemi, TypCinnosti FROM Kriminalni_cinnost;
    RADEK OBSAH%ROWTYPE;
    POCET_CINNOSTI NUMBER;
    POCET_VRAZD NUMBER;
    PROCENT_VRAZD NUMBER;
    ADRESA_UZEMI VARCHAR(30);
  BEGIN
    POCET_CINNOSTI := 0;
    POCET_VRAZD := 0;
    SELECT Adresa INTO ADRESA_UZEMI FROM Kriminalni_cinnost NATURAL JOIN Uzemi WHERE idUZEMI = ID_Uzemi;
    OPEN OBSAH;
    LOOP
      FETCH OBSAH INTO RADEK;
      EXIT WHEN OBSAH%NOTFOUND;
      IF(RADEK.ID_Uzemi = idUZEMI) THEN
        POCET_CINNOSTI := POCET_CINNOSTI + 1;
        IF(RADEK.TypCinnosti = 'Vrazda') THEN
          POCET_VRAZD := POCET_VRAZD + 1;
        end if;
      end if;
    end loop;
    PROCENT_VRAZD := (POCET_VRAZD/POCET_CINNOSTI)*100;
    DBMS_OUTPUT.PUT_LINE('Na uzemi ' || ADRESA_UZEMI || ' doslo k ' || POCET_CINNOSTI || ' kriminalnim cinnostem, z toho ' || PROCENT_VRAZD || '% vrazd.');
    EXCEPTION
      WHEN ZERO_DIVIDE THEN
        DBMS_OUTPUT.PUT_LINE('ZERO DIVISION ERROR');
      WHEN OTHERS THEN
        RAISE_APPLICATION_ERROR(-20002, 'ERROR IN percentMurder FUNCTION');
  end;


CREATE OR REPLACE PROCEDURE printMurderers
  IS
    CURSOR OBSAH1 IS SELECT ID_Cinnosti, TypCinnosti FROM Kriminalni_cinnost;
    RADEK1 OBSAH1%ROWTYPE;
    CURSOR OBSAH2 IS SELECT ID_Cinnosti, RC_Clena, Jmeno FROM CProvadiK NATURAL JOIN Clen;
    RADEK2 OBSAH2%ROWTYPE;
    JMENO VARCHAR(30);
    RODNE_CISLO VARCHAR(11);
  BEGIN
    OPEN OBSAH1;
    DBMS_OUTPUT.PUT_LINE('Seznam vrahu: ');
    LOOP
      FETCH OBSAH1 INTO RADEK1;
      EXIT WHEN OBSAH1%NOTFOUND;
      OPEN OBSAH2;
      LOOP
        FETCH OBSAH2 INTO RADEK2;
        EXIT WHEN OBSAH2%NOTFOUND;
        IF(RADEK1.ID_Cinnosti = RADEK2.ID_Cinnosti AND RADEK1.TypCinnosti = 'Vrazda') THEN
          DBMS_OUTPUT.PUT_LINE(RADEK2.RC_Clena || ' ' || RADEK2.Jmeno);
        end if;
      end loop;
      CLOSE OBSAH2;
    end loop;
  end;




------------------------------------
--         Testovací data         --
------------------------------------
--Familie
INSERT INTO Familie (ID_Familie, JmenoFamilie, JmenoDona, DatumNarozeniDona, VelikostBotDona, PohlaviDona, BarvaVlasuDona, JmenoConsigliere)
    VALUES ('1', 'Ressutana', 'Gaetano Fidanzati', date '1967-05-23', '42', 'M', '0000FF', 'Gaspare Mutolo');
INSERT INTO Familie (ID_Familie, JmenoFamilie, JmenoDona, DatumNarozeniDona, VelikostBotDona, PohlaviDona, BarvaVlasuDona, JmenoConsigliere)
    VALUES ('2', 'Corleone', 'Luciano Leggio', date '1967-05-23', '42', 'Z', '000FFF', 'Antonino Calderone');
INSERT INTO Familie (ID_Familie, JmenoFamilie, JmenoDona, DatumNarozeniDona, VelikostBotDona, PohlaviDona, BarvaVlasuDona, JmenoConsigliere)
    VALUES ('3', 'San Lorenzo', 'Giuseppe Giacomo Gambino', date '1967-05-23', '42', 'M', '0000FF', 'Dario Cadorini');

--Aliance
INSERT INTO Aliance (ID_Aliance, Aliance1, Aliance2, DatumZacatku, DatumUkonceni)
    VALUES ('1', '1', '2', date '2018-03-25', date '2018-03-29');
INSERT INTO Aliance (ID_Aliance, Aliance1, Aliance2, DatumZacatku, DatumUkonceni)
    VALUES ('2', '1', '3', date '2018-01-23', date '2018-02-06');
INSERT INTO Aliance (ID_Aliance, Aliance1, Aliance2, DatumZacatku, DatumUkonceni)
  VALUES ('3', '2', '3', date '2017-12-24', date '2018-01-01');

--Clenove
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('930524/7182', '1', 'Vito Scaletta', date '1993-05-24', 'M');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('901209/9360', '1', 'Thomas Angello', date '1990-12-09', 'M');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('526211/026', '2', 'Dominica DeCoco', date '1952-12-11', 'Z');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('531130/111', '2', 'Enzo Gorlami', date '1953-11-30', 'M');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('901113/4264', '3', 'Antonio Margheriti', date '1990-11-13', 'M');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('910214/6317', '3', 'Alonzo Arduino', date '1991-02-14', 'M');

--Capo
INSERT INTO Caporegime (RC_Clena, UrovenDuvery)
    VALUES ('930524/7182', '9000');
INSERT INTO Caporegime (RC_Clena, UrovenDuvery)
    VALUES ('526211/026', '9001');
INSERT INTO Caporegime (RC_Clena, UrovenDuvery)
    VALUES ('901113/4264', '3');

--Radovy clen
INSERT INTO Radovy_clen(RC_Clena, Oblibena_zbran)
    VALUES ('901209/9360', 'PPS');
INSERT INTO Radovy_clen(RC_Clena, Oblibena_zbran)
    VALUES ('531130/111', 'VZ58');
INSERT INTO Radovy_clen(RC_Clena, Oblibena_zbran)
    VALUES ('910214/6317', 'CZ12');

--Uzemi
INSERT INTO Uzemi(ID_Uzemi, Majitel, Adresa, Rozloha)
    VALUES ('1', '1', 'Brno-stred', '8');
INSERT INTO Uzemi(ID_Uzemi, Majitel, Adresa, Rozloha)
    VALUES ('2', '2', 'Brno-sever', '8');
INSERT INTO Uzemi(ID_Uzemi, Majitel, Adresa, Rozloha)
    VALUES ('3', '3', 'Brno-vychod', '8');
INSERT INTO Uzemi(ID_Uzemi, Majitel, Adresa, Rozloha)
    VALUES ('4', '1', 'Brno-jih', '8');

--Kriminalni cinnost
INSERT INTO Kriminalni_cinnost(ID_Cinnosti, ID_Uzemi, TypCinnosti, DatumZacatku, DatumUkonceni)
    VALUES ('1','1', 'Kradez majetku', date '2018-01-01', date '2018-02-02');
INSERT INTO Kriminalni_cinnost(ID_Cinnosti, ID_Uzemi, TypCinnosti, DatumZacatku, DatumUkonceni)
    VALUES ('2','2', 'Kradez majetku', date '2018-01-01', date '2018-02-02');
INSERT INTO Kriminalni_cinnost(ID_Cinnosti, ID_Uzemi, TypCinnosti, DatumZacatku, DatumUkonceni)
    VALUES ('3','3', 'Vrazda', date '2018-01-01', date '2018-02-02');

--Vrazdy
INSERT INTO Vrazda(ID_Cinnosti, Obet)
    VALUES ('3', 'Hans Landa');

--Provadi
INSERT INTO CProvadiK(RC_Clena, ID_Cinnosti)
    VALUES ('531130/111', '3');
INSERT INTO CProvadiK(RC_Clena, ID_Cinnosti)
    VALUES ('526211/026', '2');
INSERT INTO CProvadiK(RC_Clena, ID_Cinnosti)
    VALUES ('901113/4264', '1');

--Operuje
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('1', '1');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('1', '2');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('1', '3');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('2', '1');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('2', '2');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('2', '3');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('3', '1');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('3', '2');
INSERT INTO FOperujeU(ID_Familie, ID_Uzemi)
    VALUES ('3', '3');

--Setkani donu
INSERT INTO Setkani_donu(ID_Setkani, ID_Uzemi, DatumSetkani)
    VALUES ('1', '1', date '2018-11-01');

--Pozvany
INSERT INTO FPozvanyS(ID_Familie, ID_Setkani, Potvrdil)
    VALUES ('1', '1', '0');
INSERT INTO FPozvanyS(ID_Familie, ID_Setkani, Potvrdil)
    VALUES ('2', '1', '0');
INSERT INTO FPozvanyS(ID_Familie, ID_Setkani, Potvrdil)
    VALUES ('3', '1', '1');

--Vede
INSERT INTO FVedeK(ID_Familie, ID_Cinnosti)
    VALUES ('1', '1');
INSERT INTO FVedeK(ID_Familie, ID_Cinnosti)
    VALUES ('2', '1');
INSERT INTO FVedeK(ID_Familie, ID_Cinnosti)
    VALUES ('3', '3');
INSERT INTO FVedeK(ID_Familie, ID_Cinnosti)
    VALUES ('2', '2');


------------------------------------
--            SELECTY             --
------------------------------------
--Select spojující 2 tabulky
--zobrazi identifikator uzemi, dobu trvani vrazdy a jejiho vykonavatele
SELECT ID_Uzemi, DatumZacatku, DatumUkonceni, RC_Clena AS Vykonavatel FROM Kriminalni_cinnost, CProvadiK WHERE Kriminalni_cinnost.TypCinnosti='Vrazda' AND CProvadiK.ID_Cinnosti=Kriminalni_cinnost.ID_Cinnosti;
--zobrazi id familie, jeji jmeno a jmeno consigliereho a id setkani vsech familii, ktere prijmuly pozvani k setkani
SELECT Familie.ID_Familie, JmenoFamilie, JmenoConsigliere, ID_Setkani FROM FPozvanyS, Familie WHERE Potvrdil=1 AND Familie.ID_Familie=FPozvanyS.ID_Familie;

--SELECT spojujici 3 tabulky
--zobrazi typ kriminalni cinnnosti, majitele uzemi(tedy vykonavatele kriminalni cinnosti na danem uzemi) a jeho rozlohu.
SELECT TypCinnosti, Majitel, JmenoFamilie, Rozloha FROM Kriminalni_cinnost, Uzemi, Familie where Kriminalni_cinnost.ID_Uzemi=Uzemi.ID_Uzemi and Majitel=ID_Familie;

--zobrazi datum narozeni nejmladsiho clena z rodiny.
SELECT Clen.ID_Familie, MAX(Clen.DatumNarozeni) AS Narozeni_nejmladsiho_clena FROM Clen GROUP BY Clen.ID_Familie;
--zobrazi pocet provedeni kriminalnich cinnosti, serazeno od nejvyssiho poctu po nejnizsi.
SELECT FVedeK.ID_Familie, COUNT(FVedeK.ID_Cinnosti) AS Pocet_kriminalnich_cinosti FROM FVedeK GROUP BY FVedeK.ID_Familie ORDER BY COUNT(FVedeK.ID_Cinnosti) DESC;

-- EXISTS
-- Existuje území, na kterém probìhla kriminální èínnost?
SELECT Adresa FROM Uzemi WHERE EXISTS(SELECT Kriminalni_cinnost.ID_Uzemi FROM Kriminalni_cinnost WHERE Kriminalni_cinnost.ID_Uzemi = Uzemi.ID_Uzemi);

-- Vnoøený SELECT
-- Zobrazí èleny, kteøí nevykonali žádnou vraždu
SELECT Jmeno FROM Clen WHERE Jmeno NOT IN (SELECT Jmeno FROM Vrazda NATURAL JOIN Kriminalni_cinnost NATURAL JOIN CProvadiK NATURAL JOIN Clen);

------------------------------------
--          EXPLAIN PLAN          --
------------------------------------
-- Dotaz vypíše prùmìrné datum narození v rodinì Corleone
EXPLAIN PLAN FOR
SELECT JmenoFamilie, to_date(round(avg(to_number(to_char(DatumNarozeni, 'J')))), 'J') AS "Prumerne datum narozeni"
  FROM Familie NATURAL JOIN Clen
  WHERE JmenoFamilie = 'Corleone'
GROUP BY JmenoFamilie;

SELECT * FROM TABLE(DBMS_XPLAN.display);

------------------------------------
--              INDEX             --
------------------------------------
-- Urychlí pøedchozí dotaz
CREATE INDEX myIndex ON Familie(JmenoFamilie);
EXPLAIN PLAN FOR
SELECT JmenoFamilie, to_date(round(avg(to_number(to_char(DatumNarozeni, 'J')))), 'J') AS "Prumerne datum narozeni"
  FROM Familie NATURAL JOIN Clen
  WHERE JmenoFamilie = 'Corleone'
GROUP BY JmenoFamilie;

SELECT * FROM TABLE(DBMS_XPLAN.display);

------------------------------------
--              PRAVA             --
------------------------------------
-- Pøidìlení práv na všechny tabulky uživateli xkaspa40
GRANT ALL ON Familie TO xkaspa40;
GRANT ALL ON Setkani_donu TO xkaspa40;
GRANT ALL ON FPozvanyS TO xkaspa40;
GRANT ALL ON Clen TO xkaspa40;
GRANT ALL ON Kriminalni_cinnost TO xkaspa40;
GRANT ALL ON Vrazda TO xkaspa40;
GRANT ALL ON Caporegime TO xkaspa40;
GRANT ALL ON Radovy_clen TO xkaspa40;
GRANT ALL ON FVedeK TO xkaspa40;
GRANT ALL ON Uzemi TO xkaspa40;
GRANT ALL ON Aliance TO xkaspa40;
GRANT ALL ON CProvadiK TO xkaspa40;
GRANT ALL ON FOperujeU TO xkaspa40;
GRANT EXECUTE ON percentMurder TO xkaspa40;
GRANT EXECUTE ON printMurderers TO xkaspa40;
------------------------------------
--     MATERIALIZOVANÝ POHLED     --
------------------------------------
-- Vytvoøení MATERIALIZED VIEW
CREATE MATERIALIZED VIEW pohled
 REFRESH ON COMMIT AS -- default: BUILD IMMEDIATE
SELECT
  Familie.JmenoFamilie,
  Familie.JmenoDona FROM Familie;

--Pøidìlení práv
GRANT ALL ON pohled TO xkaspa40;

-- Dùkaz, že pohled funguje..
SELECT * FROM pohled;
INSERT INTO Familie (ID_Familie, JmenoFamilie, JmenoDona, DatumNarozeniDona, VelikostBotDona, PohlaviDona, BarvaVlasuDona, JmenoConsigliere)
    VALUES ('4', 'Quatro Formaggi', 'Lorenzo la Firence', date '1966-05-23', '43', 'M', '0000FF', 'Salvador de Fico');
INSERT INTO Clen (RC_Clena, ID_Familie, Jmeno, DatumNarozeni, Pohlavi)
    VALUES ('970223/4597', '4', 'Kebap de Balkan', date '1991-02-14', 'M');
COMMIT;
SELECT * FROM pohled;