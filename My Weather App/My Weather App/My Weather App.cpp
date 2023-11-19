#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

class WeatherData {
public:
    string city;
    double temperature;
    double humidity;
    WeatherData* next;

    WeatherData(string city, double temperature, double humidity) {
        this->city = city;
        this->temperature = temperature;
        this->humidity = humidity;
        this->next = nullptr;
    }
};

class WeatherDataList {
public:
    WeatherData* head;

    WeatherDataList() {
        head = nullptr;
    }

    void append(string city, double temperature, double humidity) {
        WeatherData* newData = new WeatherData(city, temperature, humidity);

        if (head == nullptr) {
            head = newData;
        }
        else {
            WeatherData* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newData;
        }
    }

    // Function to fetch and update weather data
    bool fetchWeatherData(string city, string apiKey) {
        CURL* curl;
        CURLcode res;
        string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey;
        cout << "URL: " << url << endl;



        string response;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                cerr << "Error: cURL request failed - " << curl_easy_strerror(res) << endl;
                return false;
            }


            try {
                json jsonData = json::parse(response);
                double temperature = jsonData["main"]["temp"];
                double humidity = jsonData["main"]["humidity"];
                append(city, temperature, humidity);
                return true;
            }
            catch (const exception& e) {
                cerr << "Error while parsing JSON: " << e.what() << endl;
                return false;
            }
        }
        else {
            cerr << "Error: cURL initialization failed" << endl;
            return false;
        }
    }

    // Function to display weather data
    void display() {
        WeatherData* current = head;
        while (current != nullptr) {
            cout << "City: " << current->city << ", Temperature: " << current->temperature << "°K, Humidity: " << current->humidity << "%" << endl;
            current = current->next;
        }
    }

    // used in conjucation with curl
    // basically to remove overflow
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
        size_t total_size = size * nmemb;
        output->append(static_cast<char*>(contents), total_size);
        return total_size;
    }
};

int main() {
    WeatherDataList weatherDataList;

    // Fetch weather data for a specific city with API KEY
    string apiKey = "1b5de72f3f1f48460bb1f7c71e9b0ed7";
    string cityToFetch = "Noida";

    if (weatherDataList.fetchWeatherData(cityToFetch, apiKey)) {
        cout << "Fetched weather data for " << cityToFetch << endl;
    }
    else {
        cerr << "Failed to fetch weather data for " << cityToFetch << endl;
    }

    // Display the updated weather data
    weatherDataList.display();

    return 0;
}
