#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <fstream>
#include<bitset>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* right;
    Node* left;

    Node(char ch, int count) : ch(ch), freq(count), right(nullptr), left(nullptr) {}
};

struct compare {
    bool operator()(Node* l, Node* r) { // Overloading the () operator
        return l->freq > r->freq;
    }
};

Node* buildHuffman(unordered_map<char, int> &mpp) {
    priority_queue<Node*, vector<Node*>, compare> pq; // Min-heap
    for (auto& it : mpp) {
        pq.push(new Node(it.first, it.second));
    }
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        Node* node = new Node('\0', sum);
        node->left = left;
        node->right = right;
        pq.push(node);
    }
    return pq.top();
}

void generatehuffman(Node* root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr) {
        return;
    }
    if (root->ch != '\0') {
        huffmanCode[root->ch] = str;
    }
    generatehuffman(root->left, str + "0", huffmanCode);
    generatehuffman(root->right, str + "1", huffmanCode);
}

string compresstext(const string &s, const unordered_map<char, string> &huffmanCode) {
    string compressed_text;
    for (char it : s) {
        compressed_text += huffmanCode.at(it);
    }
    return compressed_text;
}

void writeCompressedFile(const string& compressedText, const string& outputFile) {   //const so that function does not modify the input string
    ofstream out(outputFile,ios::binary);   //my outputfile name is outputFile which is converted into binary
    bitset<8> bits;
    int bitIndex = 0;

    for (char bit : compressedText) {
        if (bit == '1') {
            bits.set(7 - bitIndex);
        }
        bitIndex++;

        if (bitIndex == 8) {
            out.put(static_cast<unsigned char>(bits.to_ulong()));
            bits.reset();
            bitIndex = 0;
        }
    }

    if (bitIndex != 0) {                                    //this is used to check if any number is not a multiple of 8
        out.put(static_cast<unsigned char>(bits.to_ulong()));
    }

    out.close();
}

string readcompressedfile(const string &inputfile){   //input file wo compreesed.bin wali file hai
    ifstream in(inputfile,ios::binary);   //ios binary poora binary me read krega file ko not as a text
    string compressedtext2;
    char byte;

    while (in.read(reinterpret_cast<char*>(&byte), 1)) {       //address of byte is casted to char ad\nd 1 ensures only one byte at a time
        bitset<8> bits(byte);
        compressedtext2 += bits.to_string();
    }

    in.close();
    return compressedtext2;
}

string decompress_text(const string &compressed_data,Node* root){
    string decompressText;
    Node *current=root;
    for(char bit:compressed_data){
        if(bit=='0'){
            current=current->left;
        }
        else{
            current=current->right;
        }
        if(!current->left and !current->right){
        decompressText+=current->ch;
        current=root;
        }
    
    }
    return decompressText;
    
}


int main() {
    string s;
    cout << "Enter the text to be compressed: ";
    getline(cin, s);

    unordered_map<char, int> mpp;
    for (char it : s) { // Step-1: Counting the frequency of each character
        mpp[it]++;
    }

    Node* root = buildHuffman(mpp); // Step-2: Building the Huffman tree

    unordered_map<char, string> huffmanCode;
    generatehuffman(root, "", huffmanCode); // Step-3: Generating the Huffman codes

    string compressed = compresstext(s, huffmanCode); // Step-4: Compressing the text

    writeCompressedFile(compressed,"compressed.bin"); //step-5 Writing the compressed text to a file
    cout << "Text has been compressed and written to 'compressed.bin'." << endl;

    string compressed_data=readcompressedfile("compressed.bin");   //step-6 to read the compressed data for converting into original
    
    string decompress_data=decompress_text(compressed_data,root);   //step-7 decompressing the text tree ko dekh dekh kr 

    cout<<"Original Text: "<<s<<endl;
    cout<<"Decompressed Text: "<<decompress_data<<endl;





    return 0;
}
