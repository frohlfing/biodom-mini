Dieses Verzeichnis ist für Projekt-Headerdateien vorgesehen.

Eine Headerdatei ist eine Datei mit C-Deklarationen und Makrodefinitionen, die von mehreren Projektquelldateien gemeinsam genutzt werden. Man bindet sie mit der C-Präprozessor-Direktive `#include` ein:

```cpp
#include "header.h"

int main (void)

{
...
}
```

Weitere Informationen zur Verwendung von Headerdateien finden Sie in der offiziellen GCC-Dokumentation:
https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html