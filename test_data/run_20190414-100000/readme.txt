Första försök. Kort sträcka ned till vändplan och tillbaka. Max hastighet ca 35km/h.

Första analys:
Börjar på 93sec, slutar 282 -> runt 3min. Verkar kort.

Stort gap från 238 till 274. Oklart varför.

Överslag:
36km/h -> 10m/s.
Hjulomkrets ca 2.2m
-> ~5Hz rotation.

36 ekrar-> ca 180Hz. -> ca 5ms mellan pulser.

Ser pulser med 6.5ms mellan, verkar rimligt.
Ser en del tappade pulser. Mitt i 6.5ms dyker 3 st 13ms upp. Trol. tappat en del där.

Baudrate 115200. -> 11500cps. 36 tecken/rad.
-> 11520/36 = 311Hz -> 3.2ms tid för utskrift.

Påbörjat pythonskript för undersökning.
Check 1: Cnt är komplett. Inga hål finns så alla lograder finns.

Verkar fungera i stora drag. Några observationer:
- Generellt ser signalerna väldigt brusfria ut. Finns mycket information att hämta.
- En del tappade avläsningar. Syns tydligt med hastighet som då och då droppar till hälften. 
  I bland flera på rad.
- Ventilen syns tydligt. Bör vara enkel att känna igen.
- Ser en del cykliska variationer. Bör bero på olika ekeravstånd. Helt klart värt att 
  försöka tracka enskilda avstånd.


Så, sensorn fungerar relativt bra. Finns dock utrymme för förbättring, kanske 3D montage
av hållare med bättre kontroll av ljusavskärmning. 
Notera, var molnigt under test. Oklart vad solen gör.
Har rättat en bugg i uppräckning av count efter denna mätserie. Finns risk att mätningar
tagits men inte rapporterats upp.
