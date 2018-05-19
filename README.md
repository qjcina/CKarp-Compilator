![logo](icon.png)
# CKarp
## Typy danych
  - Całkowite (32-bit signed integer) `Int #nazwa_zmiennej;`
2. Zmiennoprzecinkowe (64-bit signed floating point) `Flt #nazwa_zmiennej;`
3. String `Str #nazwa_zmiennej = "text";`
4. Struktury danych __(NYI)__ `St #nazwa_zmiennej;`
  ```
   St nazwa_struktury{
     Int #x;
     Flt #y;
   };
  ```
### Literały
  - `#zmienna = 3;`
  - `Str #zmienna = "tekst";`

## Operacje na zmiennych
 - Operacje arytmetyczne: dodawanie (`+`), odejmowanie (`-`), mnożenie (`*`), dzielenie(`/`)
 - Konwersja typów:
  - `Int` -> `Flt`
  - `Flt` -> `Int`
  - `Str` nie przyjmuje żadnej konwersji
 - Wypisywanie:
  - `Flt`: `Print(#zmienna);`
  - `Int`: `Pirnt(#zmienna);`
  - `Str`: `Print(#zmienna);`
 - Wpisywanie:
  - `Flt`: `Read(#zmienna);`
  - `Int`: `Read(#zmienna);`
 - Operacja podpisania dla zmiennych tego samego typu `=`

## Tablice zmiennych
- Jednowymiarowe `Flt #nazwa_zmiennej[rozmiar];`

## Instrukcje
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
## Założenia
 - Wszyszystkie instrukcje, typy oraz nazwy funkcji wbudowanych zaczynają się z wielkiej litery.
 - Wszystkie nazwy zmiennych zaczynają się znakiem `#`
 - Każde polecenie kończy znak `;`
 - Bloki struktur, bloków warunkowych oraz pętli otoczone są znakami `{ }`
