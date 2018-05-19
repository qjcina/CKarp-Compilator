<div style="width: 250px; margin: 0 auto;"><h1>C-Carp
<img style="width: 100px;" src="icon.png"></h1></a>
# Typy danych
1. Całkowite (32-bit signed integer) `Int #nazwa_zmiennej;`
2. Zmiennoprzecinkowe (64-bit signed floating point) `Flt #nazwa_zmiennej;`
3. String `Str #nazwa_zmiennej;`
4. Struktury danych `St #nazwa_zmiennej;`
 ```
  St nazwa_struktury{
    Int #x;
    Flt #y;
  };
 ```
### Literały
  - `Int #zmienna = 3;`
  - `Flt #zmienna = 3.1;`
  - `Str #zmienna = "tekst";`

# Operacje na zmiennych
 - Operacje arytmetyczne: dodawanie (`+`), odejmowanie (`-`), mnożenie (`*`), dzielenie(`/`)
 - Konwersja typów:
  - `Int` -> `Flt` konwersja przez dodanie części ułamkowej (równej 0) <br />
  `Flt #zmienna = ItF(3);`
  - `Flt` -> `Int` konwersja przez odcięcie części ułamkowej <br />
  `Int #zmienna = FtI(3.14);`
  - `Str` nie przyjmuje żadnej konwersji
 - Wypisywanie:
  - `Flt`: `Printf(#zmienna);`
  - `Int`: `Pirnti(#zmienna);`
  - `Str`: `Prints(#zmienna);`
 - Wpisywanie:
  - `Flt`: `Readf(#zmienna);`
  - `Int`: `Readi(#zmienna);`
 - Operacja podpisania dla zmiennych tego samego typu `=`

# Tablice zmiennych
- Jednowymiarowe `Flt[rozmiar] #nazwa_zmiennej;`
- Wielowymiarowe do 3 wymiarów `Int[rozmiarX][rozmiarY][rozmiarZ] #nazwa_zmiennej;`

# Instrukcje
- Instrukcja warunkowa `If .. Else` (z wielokrotnym zagnieżdżeniem):
 ```
 If ( warunek ){

 } Else {

 }
 ```
- Pętla `While`:
 ```
 While ( warunek ){

  }
 ```
# Założenia
 - Wszyszystkie instrukcje, typy oraz nazwy funkcji wbudowanych zaczynają się z wielkiej litery.
 - Wszystkie nazwy zmiennych zaczynają się znakiem `#`
 - Każde polecenie kończy znak `;`
 - Bloki struktur, bloków warunkowych oraz pętli otoczone są znakami `{ }`
