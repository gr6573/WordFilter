#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <fstream>

struct node {
  std::vector<node*> *nextNodeVector = new std::vector<node*>;
  node *failNode = nullptr;
  char w;
  bool hasEnd = false;
  int wordNum = 0;
  node(char w) :w(w) {}

  node *findNextNode(char w)
  {
    for (auto v : *nextNodeVector)
      if (v->w == w)
        return v;

    return nullptr;
  }
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
  if (root == nullptr)
    return;

  for (auto w : filteredWord)
  {
    if (w.empty())
      continue;

    auto currentNode = root;
    int index = 0;
    while (index < w.length())
    {
      node *find = currentNode->findNextNode(w[index]);
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
      {
        currentNode->hasEnd = true;
        currentNode->wordNum = (int)w.length();
      }
    }
  }
}

void CreateACAutomation(node *root)
{
  if (root == nullptr)
    return;

  std::queue<node*> q;
  for (auto v : *root->nextNodeVector)
  {
    v->failNode = root;
    q.push(v);
  }

  while (!q.empty())
  {
    auto currentNode = q.front();
    q.pop();
    for (auto v : *currentNode->nextNodeVector)
    {
      auto failNode = currentNode->failNode;
      while (failNode != nullptr)
      {
        node *find = failNode->findNextNode(v->w);
        if (find != nullptr)
        {
          v->failNode = find;
          break;
        }

        failNode = failNode->failNode;
      }

      if (failNode == nullptr)
        v->failNode = root;

      q.push(v);
    }
  }
}

void WordFilterDFA(const std::string &input, node *root, std::string &output)
{
  output = input;
  if (input.empty() || root == nullptr || root->nextNodeVector->size() == 0)
    return;

  int index = 0;
  int lastMarkIndex = -1;
  auto currentNode = root;
  while (index < input.length())
  {
    node *find = currentNode->findNextNode(input[index]);
    while (find == nullptr && currentNode->failNode != nullptr)
    {
      currentNode = currentNode->failNode;
      find = currentNode->findNextNode(input[index]);
    }

    if (find == nullptr)
    {
      currentNode = root;
      index++;
      continue;
    }

    currentNode = find;
    if (find->hasEnd)
    {
      int start = lastMarkIndex + 1 > index - find->wordNum + 1 ? lastMarkIndex + 1 : index - find->wordNum + 1;
      for (int i = start; i <= index; i++)
        output[i] = '*';
      lastMarkIndex = index;
    }

    index++;
  }
}

double getSeconds(std::chrono::time_point<std::chrono::system_clock> &start, std::chrono::time_point<std::chrono::system_clock> &end)
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
  CreateACAutomation(filteredWordTree);
  end = std::chrono::system_clock::now();
  std::cout << "Create AC Automation time= " << getSeconds(start, end) << "s" << std::endl;

  start = std::chrono::system_clock::now();
  std::string output2;
  WordFilterDFA(input, filteredWordTree, output2);
  end = std::chrono::system_clock::now();
  std::cout << "Word Filter DFA time= " << getSeconds(start, end) << "s" << std::endl;
  std::cout << "output: " << output2 << std::endl;
  return 0;
}
