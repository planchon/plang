#include "../include/token.h"

std::ostream& operator << (std::ostream& os, Token& tok) {
    switch (tok.tok)
    {
    case TOK_EOF:
        os << "EOF";
        break;
    case TOK_DEF:
        os << "TOK_DEF, def";
        break;
    case TOK_EXTERN:
        os << "TOK_EXTERN, extern";
        break;
    case TOK_IDENTIFIER:
        os << "TOK_IDENTIFIER, " << tok.identifier;
        break;
    case TOK_NUMBER:
        os << "TOK_NUMBER, " << tok.value;
        break;
    case TOK_OPERATOR:
        os << "TOK_OPERATOR, " << tok.ope << ", " << int(tok.ope) ;
        break;
    case TOK_NULL:
        os << "TOK_NULL";
        break;
    default:
        os << "ERROR.";
        break;
    }

    return os;
}

Token::Token() {};

void Tokenizer::from_data(std::string data) {
    this->filedata = new char[data.length() + 1];
    this->file_size = data.length() + 1;
    strcpy(this->filedata, data.c_str());
    this->file_pos = 0;
}

void Tokenizer::from_file(std::string filename) {
    this->jit_console = false;
    this->filename = filename;
    this->file.open(this->filename);

    if (!this->file.is_open()) {
        std::cout << "Cannot open file" << std::endl;
        return; 
    }

    std::streampos begin, end, size;
    begin = this->file.tellg();
    this->file.seekg(0, std::ios::end);
    end = this->file.tellg();
    size = end - begin;

    // we reset the position in the file after getting the size
    this->file.seekg(0); 
    
    this->filedata = new char[size];
    
    this->file.read(this->filedata, size);
    this->file.close();

    this->file_size = int(size);
    this->file_pos = 0;
}

// get the next char in the source file buffer
// return true if a char is found
// return false if is EOF
bool Tokenizer::get_next_char() {
    char result;

    if (this->file_pos < this->file_size) {
        this->current_char = this->filedata[this->file_pos++];
    } else {
        this->current_char = EOF;
        return false;
    }

    return true;
}

// skip all the space in the source file buffer
void Tokenizer::skip_space() {
    while(isspace(this->current_char) && this->get_next_char());
}

// Get the next token and categorize it
Token Tokenizer::get_token() {
    std::string token_data;

    // get the next char in the buffer
    // kill the tokenizer if EOF
    if (!this->get_next_char()) {
        return Token(TOK_EOF);
    }

    // first byte of a file start with \0
    if (this->current_char == '\0') {
        return Token(TOK_EOF);
    }

    // in console mode, \r means end of command
    if (this->jit_console && this->current_char == '\r') {
        return Token(TOK_EOF);
    }

    // skip all the spaces
    this->skip_space();

    // the line is commented, we skip it
    if (this->current_char == '#') {
        while (this->get_next_char() && this->current_char != '\n' && this->current_char != '\r');
    }

    // get the words, variable names and keywords
    if (isalpha(this->current_char)) {
        do {
            token_data += this->current_char;
        } while(this->get_next_char() && isalnum(this->current_char));
        
        if (token_data == "def")
            return Token(TOK_DEF);

        if (token_data == "extern")
            return Token(TOK_EXTERN);

        // if the current_char is not a number the file_pos will be incr twice 
        this->file_pos--;

        return Token(TOK_IDENTIFIER, token_data);
    }

    // get the numbers
    if (isdigit(this->current_char)) {
        int valid_period = 0;

        do {
            token_data += this->current_char;
            if (current_char == '.') valid_period++;
        } while (this->get_next_char() && (isdigit(this->current_char) || (this->current_char == '.' && valid_period == 0))); // only one period in a digit

        double val = strtod(token_data.c_str(), nullptr);
        
        // if the current_char is not a number the file_pos will be incr twice 
        this->file_pos--;

        return Token(TOK_NUMBER, val);
    }

    // must be a +-*/()
    return Token(TOK_OPERATOR, this->current_char);
}   

std::vector<Token> Tokenizer::tokenizer_loop() {
    std::vector<Token> parsed;
    Token tok;
    const int MAX_TOKEN = 100;
    int token_index = 0;
    do {

        tok = this->get_token();
        parsed.push_back(tok);
    } while (tok.tok != TOK_EOF && token_index++ < MAX_TOKEN);

    return parsed;
}
