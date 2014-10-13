#include "licensing.h"

#include <dll.h>
#include <QNetworkInterface>
#include <QDebug>

using namespace CryptoPP;

Licensing::Licensing()
    : maxConnections(defMaxConnections)
{
    foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
        QString hwa = iface.hardwareAddress();
        if (hwa.split(":").length() != 6)
            continue;
        qDebug() << "LICENSE :: HWA:" << hwa;

        StringSource pubString(wt_pub, true, new HexDecoder);
        RSASS<PKCS1v15, SHA>::Verifier pub(pubString);

        VerifierFilter *verifierFilter = new VerifierFilter(pub);
        try {
            FileSource signatureFile(signatureFilename, true, new HexDecoder);
            if (signatureFile.MaxRetrievable() != pub.SignatureLength())
                return;
            SecByteBlock signature(pub.SignatureLength());
            signatureFile.Get(signature, signature.size());

            verifierFilter->Put(signature, pub.SignatureLength());
        }
        catch (FileSource::OpenErr e) {
            qWarning() << "LICENSE :: Unable to open license file!";
            return;
        }

        StringSource(hwa.toLatin1().data(), true, verifierFilter);
        if (verifierFilter->GetLastResult())
        {
            qWarning() << "LICENSE :: Accepted";
            maxConnections = 0;
        }
        else
            qWarning() << "LICENSE :: FAILED";
    }
}
