#include "lime_c_gen.h"
#include "lime.h"

void LimeCGen::compile_code_block(Node* node) {
    
}

void LimeCGen::compile_ast_to_c(Node* node, const std::string& out_file) {

    std::string result{""};
    result += "# Lime Version: " + std::string{LIME_VERSION} + "\n";

    std::cout << result << std::endl;
     
    auto out = std::ofstream(out_file);  

    out.close();

}
