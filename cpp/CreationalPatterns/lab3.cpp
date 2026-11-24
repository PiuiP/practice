#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cassert>

//BUILDER: статья TXT -> XML

// Продукт
class XmlDocument {
    std::string content;
public:
    void add(const std::string& part) { content += part; }
    std::string str() const { return content; }
};

// Builder — интерфейс этапов сборки
class IArticleBuilder {
public:
    virtual ~IArticleBuilder() = default;
    virtual void buildTitle(const std::string& title) = 0;
    virtual void buildAuthors(const std::string& authors) = 0;
    virtual void buildText(const std::string& text) = 0;
    virtual void buildHash(const std::string& hash) = 0;
    virtual XmlDocument getResult() = 0;
};

// ConcreteBuilder
class XmlArticleBuilder : public IArticleBuilder {
    XmlDocument doc;
public:
    XmlArticleBuilder() { doc.add("<article>\n"); }
    void buildTitle(const std::string& t)   override { doc.add("  <title>" + t + "</title>\n"); }
    void buildAuthors(const std::string& a) override { doc.add("  <authors>" + a + "</authors>\n"); }
    void buildText(const std::string& t)    override { doc.add("  <text>" + t + "</text>\n"); }
    void buildHash(const std::string& h)    override { doc.add("  <hash>" + h + "</hash>\n"); }
    XmlDocument getResult() override { doc.add("</article>\n"); return doc; }
};

// Director — задаёт порядок этапов и проверяет статью
class ArticleDirector {
public:
    static std::string computeHash(const std::string& text) {   // простой хеш: h = h*31 + символ
        unsigned int h = 0;
        for (unsigned char c : text) h = h * 31u + c;
        return std::to_string(h);
    }
    bool construct(const std::vector<std::string>& lines, IArticleBuilder& builder) {
        if (lines.size() < 4) return false;           // заголовок, авторы, текст, хеш
        std::string body;                             // текст = строки 3..предпоследняя
        for (std::size_t i = 2; i + 1 < lines.size(); ++i) {
            if (i > 2) body += "\n";
            body += lines[i];
        }
        if (computeHash(body) != lines.back()) return false;   // проверка хеш-кода
        builder.buildTitle(lines[0]);
        builder.buildAuthors(lines[1]);
        builder.buildText(body);
        builder.buildHash(lines.back());
        return true;
    }
};

// Клиент паттерна Builder
class Client {
public:
    bool convertToXml(std::istream& in, std::string& xml) {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        XmlArticleBuilder builder;
        ArticleDirector director;
        if (!director.construct(lines, builder)) return false;
        xml = builder.getResult().str();
        return true;
    }
};

// ABSTRACT FACTORY: кинопрокат (звук + субтитры)

// Абстрактные продукты
class IAudioTrack {
public:
    virtual ~IAudioTrack() = default;
    virtual std::string language() const = 0;
};
class ISubtitles {
public:
    virtual ~ISubtitles() = default;
    virtual std::string language() const = 0;
};

// Конкретные продукты
class EnglishAudio     : public IAudioTrack { public: std::string language() const override { return "English"; } };
class RussianAudio     : public IAudioTrack { public: std::string language() const override { return "Russian"; } };
class EnglishSubtitles : public ISubtitles  { public: std::string language() const override { return "English"; } };
class RussianSubtitles : public ISubtitles  { public: std::string language() const override { return "Russian"; } };

// Абстрактная фабрика — создаёт согласованное семейство продуктов
class IMediaFactory {
public:
    virtual ~IMediaFactory() = default;
    virtual IAudioTrack* createAudio() = 0;
    virtual ISubtitles*  createSubtitles() = 0;
};
class EnglishFactory : public IMediaFactory {
public:
    IAudioTrack* createAudio()     override { return new EnglishAudio(); }
    ISubtitles*  createSubtitles() override { return new EnglishSubtitles(); }
};
class RussianFactory : public IMediaFactory {
public:
    IAudioTrack* createAudio()     override { return new RussianAudio(); }
    ISubtitles*  createSubtitles() override { return new RussianSubtitles(); }
};

// Клиент паттерна Abstract Factory
class RentalOrder {
    std::string film;
    IAudioTrack* audio;
    ISubtitles*  subtitles;
public:
    RentalOrder(const std::string& filmTitle, IMediaFactory& factory)
        : film(filmTitle), audio(nullptr), subtitles(nullptr) {
        setLanguage(factory);
    }
    ~RentalOrder() { delete audio; delete subtitles; }

    // смена языка: звуковая дорожка и субтитры меняются вместе
    void setLanguage(IMediaFactory& factory) {
        delete audio; delete subtitles;
        audio = factory.createAudio();
        subtitles = factory.createSubtitles();
    }
    std::string audioLanguage() const { return audio->language(); }
    std::string subtitlesLanguage() const { return subtitles->language(); }
    void print() const {
        std::cout << "Film: " << film << " | audio: " << audioLanguage()
                  << " | subtitles: " << subtitlesLanguage() << "\n";
    }
};

// FACTORY METHOD: фигуры тетриса

// Product
class Piece {
public:
    virtual ~Piece() = default;
    virtual std::string getName() const = 0;
    virtual int getCells() const = 0;
};
class IPiece : public Piece { public: std::string getName() const override { return "I"; } int getCells() const override { return 4; } };
class OPiece : public Piece { public: std::string getName() const override { return "O"; } int getCells() const override { return 4; } };
class TPiece : public Piece { public: std::string getName() const override { return "T"; } int getCells() const override { return 4; } };
class LPiece : public Piece { public: std::string getName() const override { return "L"; } int getCells() const override { return 4; } };
// супер-фигуры (клеток больше)
class PlusPiece      : public Piece { public: std::string getName() const override { return "Plus"; }      int getCells() const override { return 5; } };
class BigSquarePiece : public Piece { public: std::string getName() const override { return "BigSquare"; } int getCells() const override { return 9; } };

// Creator
class PieceCreator {
public:
    virtual ~PieceCreator() = default;
    virtual Piece* createPiece() = 0; // фабричный метод
};
class StandardPieceCreator : public PieceCreator {
    std::mt19937 rng;
public:
    StandardPieceCreator() : rng(12345) {}
    Piece* createPiece() override {
        switch (rng() % 4) {
        case 0: return new IPiece();
        case 1: return new OPiece();
        case 2: return new TPiece();
        default: return new LPiece();
        }
    }
};
class SuperPieceCreator : public PieceCreator {
    std::mt19937 rng;
public:
    SuperPieceCreator() : rng(54321) {}
    Piece* createPiece() override {
        if (rng() % 2 == 0) return new PlusPiece();
        return new BigSquarePiece();
    }
};

// Клиент паттерна фабричного метода.
class Game {
    std::mt19937 rng;
    StandardPieceCreator standardCreator;
    SuperPieceCreator superCreator;
public:
    Game() : rng(2026) {}
    Piece* nextPiece() {
        if (rng() % 5 == 0) return superCreator.createPiece(); // ~20% супер-фигур
        return standardCreator.createPiece();
    }
};

// ==================== ТЕСТИРОВАНИЕ ====================

void testBuilder() {
    std::cout << "=== Builder: TXT -> XML ===\n";
    std::string body = "Builder separates object construction into steps.\nSecond line of the article text.";
    std::string hash = ArticleDirector::computeHash(body);

    { std::ofstream f("article.txt");  // готовим корректный TXT
      f << "Builder Pattern\nMinok A.A.\n" << body << "\n" << hash << "\n"; }

    Client client;
    std::string xml;
    std::ifstream in("article.txt");
    bool ok = client.convertToXml(in, xml);
    assert(ok);
    assert(xml.find("<title>Builder Pattern</title>") != std::string::npos);
    assert(xml.find("<hash>" + hash + "</hash>") != std::string::npos);
    std::cout << xml;
    { std::ofstream out("article.xml"); out << xml; }

    std::string tmp;
    std::istringstream badHash("Title\nAuthor\nText\n12345\n"); // неверный хеш
    assert(!client.convertToXml(badHash, tmp));
    std::istringstream tooShort("Title\nAuthor\n"); // меньше 4 строк
    assert(!client.convertToXml(tooShort, tmp));
    std::cout << "Builder: all tests passed\n\n";
}

void testAbstractFactory() {
    std::cout << "=== Abstract Factory: film rental ===\n";
    EnglishFactory english;
    RussianFactory russian;

    RentalOrder order("Matrix", english);
    order.print();
    assert(order.audioLanguage() == "English");
    assert(order.subtitlesLanguage() == "English");

    order.setLanguage(russian);
    order.print();
    assert(order.audioLanguage() == "Russian");
    assert(order.subtitlesLanguage() == "Russian");
    assert(order.audioLanguage() == order.subtitlesLanguage()); // языки всегда совпадают
    std::cout << "Abstract Factory: all tests passed\n\n";
}

void testFactoryMethod() {
    std::cout << "=== Factory Method: tetris pieces ===\n";
    Game game;
    int superCount = 0;
    for (int i = 0; i < 1000; ++i) {
        Piece* p = game.nextPiece();
        if (i < 10) std::cout << p->getName() << " (" << p->getCells() << " cells)\n";
        assert(p->getCells() == 4 || p->getCells() == 5 || p->getCells() == 9);
        if (p->getCells() > 4) ++superCount;
        delete p;
    }
    std::cout << "Super pieces out of 1000: " << superCount << "\n";
    assert(superCount > 0 && superCount < 1000); // встречаются оба вида создателей
    std::cout << "Factory Method: all tests passed\n";
}

int main(int argc, char* argv[]) {
    testBuilder();
    testAbstractFactory();
    testFactoryMethod();

    if (argc > 1) {  // можно передать свой TXT-файл
        std::ifstream f(argv[1]);
        Client c; std::string xml;
        if (c.convertToXml(f, xml)) { std::cout << "\n" << xml; std::ofstream out("article.xml"); out << xml; }
        else std::cout << "\nFile check failed: wrong format or hash code\n";
    }
    std::cout << "\nAll tests passed\n";
    return 0;
}