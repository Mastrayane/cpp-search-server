# cpp-search-server
Финальный проект: поисковый сервер
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>

 
using namespace std;
 
 
const int MAX_RESULT_DOCUMENT_COUNT = 5;
 
string ReadLine() {
 string s;
 getline(cin, s);
    return s;
}
 
int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}
 
vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    words.push_back(word);
    
    return words;
}
    
struct Document {
    int id;
    double relevance;
};
 
class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
    
    void AddDocument(int document_id, const string& document) {
        /* заносим документ в "количество документов" */
         ++document_count_;
        /* Разбиваем строку документа на вектор строк из отдельных слов,
        чтобы для каждого из них заранее высчитать TF*/
        const vector<string> words = SplitIntoWordsNoStop(document);
        /* вычисляем TF каждого входящего слова */
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
    }
 
    vector<Document> FindTopDocuments(const string& raw_query) const {        
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);
        
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }
    
private:
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    int document_count_ = 0;
    
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    
    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };
    
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }
    
    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
    }
    
    struct Query {
        vector<string> plus_words;
        vector<string> minus_words;
    };
    
    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.push_back(query_word.data);
                } else {
                    query.plus_words.push_back(query_word.data);
                }
            }
        }
        return query;
    }
 
    /*Заготоавливаем функцию для вычисления IDF*/
    double IDF(const string& word) const {
        return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
    }
    
    vector<Document> FindAllDocuments(const Query& query) const {
        map<int, double> document_to_relevance;
        /*Проходимся по плюс-словам поискового запроса*/
        for (const string& word : query.plus_words) {
            /*Если плюс-слово отсутствует в документах переходим на новую иттерацию*/
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            /*Если плюс-слово присутствует в документах вычисляем его IDF с помощью заготовленной функции
            и сохраняем в переменную*/
            const double i_d_f = IDF(word);
            /*Вычисляем общую релевантность слова умножив его TF и IDF* и сохраняем её значение в document_to_relevance*/
            for (const auto [document_id, TF] : word_to_document_freqs_.at(word)) {
                document_to_relevance[document_id] += TF * i_d_f;
            }
        }
        
  /* Проверяем в собрании документов наличие минус-слов указанных в строке запроса */
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            /*Если минус-слово найлено очищаем от него document_to_relevance*/
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }
 /*Объявляем вектор пар и переносим в него данные из контейнера map document_to_relevance*/
        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({document_id, relevance});
        }
        return matched_documents;
    }
};
 
 
SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
 
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    
    return search_server;
}
 
 
int main() {
    const SearchServer search_server = CreateSearchServer();
 
    const string query = ReadLine();
    for (auto [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s
             << endl;
    }
 
    return 0;
}

