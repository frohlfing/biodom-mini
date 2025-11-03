Dieses Verzeichnis ist für Projekt-Headerdateien vorgesehen.

Eine Headerdatei ist eine Datei mit C-Deklarationen und Makrodefinitionen, die von mehreren Projektquelldateien gemeinsam genutzt werden. Sie binden eine Headerdatei in Ihre Projektquelldatei (C, C++ usw.) im Ordner `src` ein, indem Sie sie mit der C-Präprozessor-Direktive `#include` einbinden.

```cpp
#include "header.h"

int main (void)

{
...
}
```

Das Einbinden einer Headerdatei führt zum gleichen Ergebnis wie das Kopieren der Headerdatei in jede benötigte Quelldatei. Dieses Kopieren wäre zeitaufwändig und fehleranfällig. Mit einer Headerdatei befinden sich die zugehörigen Deklarationen nur an einer Stelle. Sollten sie geändert werden müssen, können sie an einer Stelle geändert werden, und Programme, die die Headerdatei einbinden, verwenden beim nächsten Kompilieren automatisch die neue Version. Die Headerdatei erspart das mühsame Suchen und Ändern aller Kopien sowie das Risiko, dass das Fehlen einer Kopie zu Inkonsistenzen im Programm führt.

In C ist es üblich, Headerdateien mit der Endung `.h` zu benennen.

Weitere Informationen zur Verwendung von Headerdateien finden Sie in der offiziellen GCC-Dokumentation:

* Include-Syntax
* Include-Operation
* Einmalige Header
* Berechnete Includes

https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html