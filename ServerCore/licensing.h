#ifndef LICENSING_H
#define LICENSING_H

static const char *wt_pub =
        "30820120300D06092A864886F70D01010105000"
        "382010D00308201080282010100BCC1B501CCDF"
        "483C93F5267EDD42C814A6F7148CC72D2C8577B"
        "444A8C146672714E0D4930DD8DED40A6315D969"
        "7155C4D842EFD9C07D5A95015D49E598C55B63E"
        "7BE654036F6CE28673D492E62DFC8A995D49433"
        "49A9C90CF4CB7CC6F2A5B009D9BBF70A5647598"
        "EBDBD4A1E0CFC3C0AB36F62D25259355A4B6DDA"
        "3F86E3F3CEBB17E9572048D4343B06DBB62081D"
        "22FA2CF98D14FBEC3471E397A0C622DC3D6518F"
        "9769E33335169DF6F20D195B3E212A06C6A6331"
        "E98788BC7EEF59E9DFFE9E9B0442F0110394FA0"
        "5ADED7B2007CF58AC381A94F3DF866DB00C0AD8"
        "A9C077CB50B4A477C71D6C8458BD76EF232E070"
        "48C95DD1AB51A5C5871F676F7BC60EA5020111";

static const char *signatureFilename = "license.sig";
static const int defMaxConnections   = 20;

class Licensing
{
public:
    Licensing();
    inline int getMaxConnections()
    {
        return maxConnections;
    }

private:
    int maxConnections;
};

#endif // LICENSING_H