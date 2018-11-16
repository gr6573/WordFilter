#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>

struct node {
  std::vector<node*> *nextNodeVector = new std::vector<node*>;
  char w;
  bool hasEnd = false;
  node(char w) :w(w) {}
};

void WordFilterNormal(const std::string &input, const std::vector<std::string> &filteredWord, std::string &output)
{
  output = input;
  if (input.empty())
    return;

  for (auto w : filteredWord)
  {
    if (w.length() > input.length())
      continue;

    auto pos = input.find(w);
    while (pos != std::string::npos)
    {
      output.replace(pos, w.length(), w.length(), '*');
      pos = input.find(w, pos + 1);
    }
  }
}

void CreateFilteredWordTree(const std::vector<std::string> &filteredWord, node *root)
{
  if (root == nullptr || root->nextNodeVector == nullptr)
    return;

  for (auto w : filteredWord)
  {
    if (w.empty())
      continue;

    auto currentNode = root;
    int index = 0;
    while (index < w.length() && currentNode->nextNodeVector != nullptr)
    {
      node *find = nullptr;
      for (auto v : *currentNode->nextNodeVector)
      {
        if (v->w == w[index])
        {
          find = v;
          break;
        }
      }

      if (find != nullptr)
        currentNode = find;
      else
      {
        auto newNode = new node(w[index]);
        newNode->nextNodeVector->reserve(3);
        currentNode->nextNodeVector->emplace_back(newNode);
        currentNode = newNode;
      }

      index++;
      if (index == w.length())
        currentNode->hasEnd = true;
    }
  }
}

void WordFilterDFA(const std::string &input, const node *root, std::string &output)
{
  output = input;
  if (input.empty() || root == nullptr || root->nextNodeVector == nullptr || root->nextNodeVector->size() == 0)
    return;

  for (int start = 0; start < input.length(); start++)
  {
    int end = start;
    auto currentNode = root;
    int index = start;
    while (index < input.length() && currentNode->nextNodeVector != nullptr)
    {
      node *find = nullptr;
      for (auto v : *currentNode->nextNodeVector)
      {
        if (v->w == input[index])
        {
          find = v;
          break;
        }
      }

      if (find == nullptr)
        break;

      currentNode = find;
      index++;
      if (find->hasEnd)
        end = index;
    }

    for (int i = start; i < end; i++)
      output[i] = '*';
  }
}

double getSeconds(std::chrono::time_point<std::chrono::system_clock> start, std::chrono::time_point<std::chrono::system_clock> end)
{
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return double(duration.count()) / 1000000;
}

int main()
{
  auto start = std::chrono::system_clock::now();
  std::ifstream ifs;
  std::vector<std::string> filteredWord;
  filteredWord.reserve(200000);
  std::string word;
  for (int i = 0; i < 8; i++)
  {
    ifs.open("key.txt");
    if (!ifs)
      return 1;

    while (std::getline(ifs, word))
      filteredWord.emplace_back(word);

    ifs.close();
  }
  auto end = std::chrono::system_clock::now();
  std::cout << "Create filteredWord num= " << filteredWord.size() << " time= " << getSeconds(start, end) << "s" << std::endl;

  std::string input;
  std::string output;
  ifs.open("input.txt");
  if (!ifs)
    return 1;

  while (std::getline(ifs, word))
    input += word;

  ifs.close();
  std::cout << "input: " << input << std::endl << std::endl;

  start = std::chrono::system_clock::now();
  WordFilterNormal(input, filteredWord, output);
  end = std::chrono::system_clock::now();
  std::cout << "Word Filter Normal time= " << getSeconds(start, end) << "s" << std::endl;
  std::cout << "output: " << output << std::endl << std::endl;

  start = std::chrono::system_clock::now();
  auto filteredWordTree = new node('\0');
  filteredWordTree->nextNodeVector->reserve(256);
  CreateFilteredWordTree(filteredWord, filteredWordTree);
  end = std::chrono::system_clock::now();
  std::cout << "Create Filtered Word Tree time= " << getSeconds(start, end) << "s" << std::endl;

  start = std::chrono::system_clock::now();
  std::string output2;
  WordFilterDFA(input, filteredWordTree, output2);
  end = std::chrono::system_clock::now();
  std::cout << "Word Filter DFA time= " << getSeconds(start, end) << "s" << std::endl;
  std::cout << "output: " << output2 << std::endl;
  return 0;
}
