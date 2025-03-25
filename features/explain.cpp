#include <cstdlib>      // for getenv
#include <iostream>
#include <string>
#include "../openai/nlohmann/json.hpp"  // Ensure this header is in your include path
#include "../include/explain.h"
#include "../openai/openai.hpp"

using nlohmann::json;

std::string explainCommand(const std::string& command) {
    const char* tokenEnv = std::getenv("GITHUB_AI_API_KEY");
    if (!tokenEnv) {
        std::cerr << "Error: GITHUB_TOKEN not set." << std::endl;
        return "Error: API key not set.";
    }
    std::string token(tokenEnv);
    // Initialize the client using your updated base URL
    openai::start(token, "", true, "https://models.inference.ai.azure.com/");
    
    // Construct the JSON payload for GPT-4o:
    json payload;
    payload["model"] = "gpt-4o";
    payload["messages"] = json::array({
        { {"role", "system"}, {"content", "You are a helpful assistant who needs to provide a very concise explanation to a beginner."} },
        { {"role", "user"}, {"content", "Explain the following command within a maximum of two brief sentences. Explain the keyword and then the argument passed by the user: " + command + ". Do not use any markdown in your answer, format it in plain text."} }
    });
    payload["max_tokens"] = 100;
    payload["temperature"] = 0.3;

    // debug: print the JSON payload
    // std::cout << "JSON Payload: " << payload.dump() << std::endl;

    json response;
    try {
        response = openai::chat().create(payload);
        // debug: print the API response
        // std::cout << "API Response: " << response.dump(2) << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error during API call: " << e.what() << std::endl;
        return "Error during API call.";
    }

    std::string explanation;
    try {
        // For GPT-4o the answer should be in response["choices"][0]["message"]["content"]
        explanation = response["choices"][0]["message"]["content"].get<std::string>();
    } catch (std::exception &e) {
        std::cerr << "Error parsing API response: " << e.what() << std::endl;
        explanation = "Error: Unable to parse API response.";
    }

    return explanation;
}