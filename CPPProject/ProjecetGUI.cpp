#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class ImageDisplay : public QWidget
{
    Q_OBJECT

public:
    ImageDisplay(const QString& imageUrl, QWidget* parent = nullptr)
        : QWidget(parent), imageUrl(imageUrl)
    {
        // Create a QLabel to display the image
        imageLabel = new QLabel(this);
        imageLabel->setAlignment(Qt::AlignCenter);
        
        // Initialize the network manager
        networkManager = new QNetworkAccessManager(this);
        
        // Make the network request to fetch the image
        QNetworkRequest request(imageUrl);
        networkManager->get(request);
        
        // Connect signals for network request completion
        connect(networkManager, &QNetworkAccessManager::finished, this, &ImageDisplay::onNetworkRequestFinished);
    }

private slots:
    void onNetworkRequestFinished(QNetworkReply* reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            // Load the image from the network reply
            QPixmap image;
            image.loadFromData(reply->readAll());
            
            // Set the image on the QLabel
            imageLabel->setPixmap(image);
        }
        else
        {
            // Handle network request errors
            imageLabel->setText("Error loading image.");
        }
        
        reply->deleteLater();
    }

private:
    QString imageUrl;
    QLabel* imageLabel;
    QNetworkAccessManager* networkManager;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // Specify the image URL here
    QString imageUrl = "https://example.com/your-image.jpg";
    
    ImageDisplay imageDisplay(imageUrl);
    imageDisplay.show();

    return app.exec();
}

#include "main.moc"
