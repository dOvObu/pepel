#ifndef PARSER_TABLE_GENERATOR_H
#define PARSER_TABLE_GENERATOR_H

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace tabgen
{

using namespace std;

struct Symbol {
    Symbol(std::string val = "", bool isTerminal = true) : IsTerminal(isTerminal), Val(val) { if (Val.size() == 3 && Val[0] == '\'' && Val[2] == '\'') Val = Val[1]; }
    bool IsTerminal{ true };
    std::string Val{  ""  };
};


struct BNF {
    template <typename T>
    using Vec = std::vector<T>;
    using Str = std::string;
    template <typename T>
    using Dict = std::map<Str, T>;
    using SymbolFromRule = std::pair<Symbol, size_t>;
    using Word = Vec<Symbol>;
    using LongRule = std::pair<Str, Vec<Word>>;
    using Rule  = std::pair<Str, Word>;


    BNF(Vec<LongRule> longRules)
    {
        _ShowLongRules(longRules);
        _ShortenRules(longRules);
        Dict<Vec<SymbolFromRule>> first = _Firsts();
        ShowRules();
        _ShowFirsts(first);
        _AddEntryRule(longRules[0].first);
        Dict<Vec<Symbol>> follow = _Follows(first);
        _ShowFollows(follow);
        _ShowPredict(first);
        Dict<Dict<size_t>> table = _Table(first, follow);
        _ShowTable(table);
    }


    void ShowRules()
    {
        for(auto& rule : _rules)
        {

            std::cout << rule.first << " ->";
            for (auto& symbol : rule.second)
            {
                std::cout << " " << symbol.Val;
            }
            std::cout << '\n';

        }
        std::cout << '\n';
    }

private:
    Vec<Rule> _rules;


    void _ShortenRules(Vec<LongRule>& rules)
    {
        std::set<Str> withEmptyRhs;

        for (auto& rule : rules)
        {
            Vec<Str> nonTerminals;

            for (auto& disjunctionItem : rule.second)
            {

                _rules.push_back({ rule.first, disjunctionItem });
                auto& last = _rules.back();

                if (!last.second.empty() && !last.second[0].IsTerminal && last.first == last.second[0].Val)
                {
                    last.first += "'";
                    last.second.erase(std::begin(last.second));
                    nonTerminals.push_back(last.first);
                }

                if (!nonTerminals.empty())
                {
                    _rules.back().second.push_back({ nonTerminals.back(), false });
                }

                if (withEmptyRhs.count(_rules.back().first) == 0 && _rules.back().first.back() == '\'')
                {
                    _rules.push_back({_rules.back().first, {}});
                    withEmptyRhs.insert(_rules.back().first);
                }

            }

        }
    }

    
    void _AddEntryRule(std::string& entryNonTerminal)
    {
        _rules.insert(std::begin(_rules), {"S",{ {entryNonTerminal, false}, {"$"} }});
    }
    

    Dict<Vec<SymbolFromRule>> _Firsts()
    {
        Dict<Vec<SymbolFromRule>> first;

        for (size_t idx = 0; _rules.size() > idx; ++idx)
        {
           auto firsts = _First(idx);
           auto& concreteFirst = first[_rules[idx].first];
           concreteFirst.insert(std::begin(concreteFirst), std::begin(firsts), std::end(firsts));
        }

        return first;
    }


    Vec<SymbolFromRule> _First(size_t idx)
    {
        auto& rule = _rules[idx];
        if (rule.second.empty())
        {
            return {{{"nil"}, idx}};
        }
        
        Vec<SymbolFromRule> result;
        for (size_t jdx = 0; rule.second.size() > jdx; ++jdx)
        {
            
            auto& currChar = rule.second[jdx];
            if (currChar.IsTerminal)
            {
                result.push_back({currChar, idx});
                break;
            }
            
            bool nilIsFound = false;
            
            for (size_t kdx = 0; _rules.size() > kdx; ++kdx)
            {
                
                if (_rules[kdx].first == currChar.Val)
                {    
                    auto iResult = _First(kdx);
                    for (auto& ires: iResult)
                    {
                        ires.second = idx;
                    }
                    result.insert(std::end(result), std::begin(iResult), std::end(iResult));
                    
                    if (_Containes(iResult, "nil"))
                    {
                        nilIsFound = true;
                    }
                }
                
            }
            
            if (!nilIsFound)
            {
                break;
            }
            
        }

        return result;
    }


    void _ShowFirsts(Dict<Vec<SymbolFromRule>> firsts)
    {
        for (auto& [key, val] : firsts)
        {
            std::cout << "first(" << key << ") = {";
            bool isNotFirst{ false };
            for (auto& [sym, idx] : val)
            {
                if (isNotFirst)
                {
                    std::cout << ", ";
                }
                else
                {
                    isNotFirst = true;
                }
                std::cout << sym.Val;
            }
            std::cout << "}\n";
        }
        std::cout << '\n';
    }


    Dict<Vec<Symbol>> _Follows(Dict<Vec<SymbolFromRule>>& firsts)
    {
        Dict<Vec<Symbol>> result;
        Vec<std::pair<size_t, size_t>> delayedShare;
       
        for (size_t currWordIdx = 0; _rules.size() > currWordIdx; ++currWordIdx)
        {
            
            auto& currWord = _rules[currWordIdx].second;
            
            // ищем текущий каждый символ в текущем слове
            for (size_t nonTermIdx = 0; _rules.size() > nonTermIdx; ++nonTermIdx)
            {
                auto& nonTerm = _rules[nonTermIdx].first;
                size_t nonTermIdxInCurrWord = 0;
                
                // если нашли символ в текущем слове
                if (_ContainesAtIndex(currWord, nonTerm, /* ==>> */ nonTermIdxInCurrWord))
                {
                    //std::cout << "                   " << nonTerm << " " << nonTermIdx << " is found\n";
                    auto lastIdxInCurrWord = currWord.size() - 1;
                    
                    for(;;)
                    {
                        //std::cout << std::string(2 * (nonTermIdxInCurrWord + 1), ' ') << "^ : " << 1+nonTermIdxInCurrWord << " ";
                        bool eatNext = false;
                        
                        // если нашли на последнем месте, то делимся следованием текущего слова со следованием символа
                        if (nonTermIdxInCurrWord == lastIdxInCurrWord)
                        {
                            if (_rules[currWordIdx].first != _rules[nonTermIdx].first)
                            {
                                delayedShare.push_back({ currWordIdx, nonTermIdx });
                            }
                            //std::cout << "is last\n                   so " << _rules[currWordIdx].first << " shares with " << _rules[nonTermIdx].first << (_rules[currWordIdx].first == _rules[nonTermIdx].first? " which is tautology\n":"\n");
                            //std::cin.get();
                            break;
                        }
                        else
                        {
                            auto& nextSymInCurrWord = currWord[nonTermIdxInCurrWord + 1];
                            //std::cout << "is " << (nextSymInCurrWord.IsTerminal? "term" : "non-term") << "\n";
                            //std::cin.get();
                            // если дальше есть ещё символ, то добавляем его
                            if (nextSymInCurrWord.IsTerminal)
                            {
                                if (!_Containes(result[nonTerm], nextSymInCurrWord.Val))
                                {
                                    result[nonTerm].push_back(nextSymInCurrWord);
                                }
                                //std::cout << "                   so we put " << nextSymInCurrWord.Val << " in Follow(" << nonTerm << ")\n";
                                //std::cin.get();
                                break;
                            }
                            // а если ссылка на слово, то добавляем его первые символы
                            else
                            {
                                //std::cout << "                   so we copy First(" << nextSymInCurrWord.Val << ") to Follow(" << nonTerm << ")\n";
                                //std::cin.get();
                                for (auto& [sym, srcIdx] : firsts[nextSymInCurrWord.Val])
                                {
                                    // все, кроме пустых
                                    if (sym.Val != "nil")
                                    {
                                        if (!_Containes(result[nonTerm], sym.Val))
                                        {
                                            result[nonTerm].push_back(sym);
                                        }
                                    }
                                    // вместо пустых обрабатываем следующий символ
                                    else
                                    {
                                        eatNext = true;
                                    }
                                    
                                }
                            }
                            
                            if (eatNext)
                            {
                                ++nonTermIdxInCurrWord;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                
            }
            
        }
        
        
        Dict<Vec<Symbol>> iResult;
        
        for (int i = 0; 2 > i; ++i)
        {
            for (auto& [src, dst] : delayedShare)
            {
                auto& srcKey = _rules[src].first;
                auto& dstKey = _rules[dst].first;
                
                if (srcKey != dstKey)
                {
                    for (auto& s : result[srcKey])
                    {
                        auto& dstVec = result[dstKey];
                        if (!_Containes(dstVec, s.Val))
                        {
                            dstVec.push_back(s);
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
    
    void _ExtendWith(Vec<Symbol>& val, Vec<Symbol>& data)
    {
        for (auto& r : data)
        {
            bool needToAdd = true;
            for (auto& v : val)
            {
                if (v.Val == r.Val)
                {
                    needToAdd = false;
                    break;
                }
            }
            if (needToAdd)
            {
                val.push_back(r);
            }
        }
    }
   
    
    void _ShowFollows(Dict<Vec<Symbol>>& follow)
    {
        for (auto& [key, val] : follow)
        {
            std::cout << "follow(" << key << ") = {";
            bool isNotFirst{ false };
            for (auto& sym : val)
            {
                if (isNotFirst)
                {
                    std::cout << ", ";
                }
                else
                {
                    isNotFirst = true;
                }
                std::cout << sym.Val;
            }
            std::cout << "}\n";
        }
        std::cout << '\n';
    }
    
    
    Dict<Dict<size_t>> _Table(Dict<Vec<SymbolFromRule>>& first, Dict<Vec<Symbol>>& follow)
    {
        Dict<Dict<size_t>> table;
        for (auto& [key, ft] : first)
        {
            for (auto& f : ft)
            {
                if (f.first.Val != "nil")
                {
                    table[key][f.first.Val] = f.second + 1;
                }
                else
                {
                    for (auto& fl : follow[key])
                    {
                        table[key][fl.Val] = f.second + 1;
                    }
                }
            }
        }
        return table;
    }
    
    
    std::string _Word2String(Word& word)
    {
        std::string res = "{";
        for (auto& c : word)
        {
            res += " ";
            res += c.Val;
        }
        res += " }";
        
        return res;
    }
    
    
    void _ShowPredict(Dict<Vec<SymbolFromRule>>& predict)
    {
        std::cout << "predicts :::\n";
        std::vector<std::string> pred(_rules.size());
        for (auto& [key, vec] : predict)
        {
            for (auto& [sym, rule] : vec)
            {
                pred[rule+1] += " " + sym.Val;
            }
        }
        for (size_t idx = 0; pred.size() > idx; ++idx)
        {
            std::cout << idx << " | " << pred[idx] << "\n";
        }
        std::cout << "::: predicts\n\n";
    }
    
    
    void _ShowTable(Dict<Dict<size_t>>& table)
    {
        std::cout << "table :::\n";
        for (auto& [nonTerm, val] : table)
        {
            std::cout << nonTerm << " :\n";
            for (auto& [term, val2] : val)
            {
                std::cout << "   " << term << " : " << _Word2String(_rules[val2].second) << "\n";
            }
        }
        std::cout << "::: table\n\n";
    }
    
    
    bool _Containes(Vec<SymbolFromRule> const& word, std::string val)
    {
        bool result = false;
        for (auto s : word) if (s.first.Val == val) { result = true; break; }
        return result;
    }
    
    
    bool _Containes(Vec<Symbol> const& word, std::string val)
    {
        bool result = false;
        for (auto s : word) if (s.Val == val) { result = true; break; }
        return result;
    }
    
    
    bool _ContainesAtIndex(Vec<Symbol> const& word, std::string val, size_t& idx)
    {
        bool result = false;
        idx = 0;
        for (auto s : word) { if (s.Val == val) { result = true; break; } ++idx; }
        return result;
    }
    
    
    void _ShowLongRules(Vec<LongRule>& rules)
    {
        for(auto& rule : rules)
        {

            std::cout << rule.first << " ->";
            bool notFirst { false };
            for (auto& word : rule.second)
            {
                if (notFirst)
                {
                    std::cout << " |";
                }
                else
                {
                    notFirst = true;
                }
                for (auto& symbol : word)
                {
                    std::cout << " " << symbol.Val;
                }
            }
            std::cout << '\n';

        }
        std::cout << '\n';
    }

};
#define read {{
#define x ,
#define Or }, {
#define nt(NAME) Symbol{#NAME, false}
#define t(NAME) Symbol{#NAME, true}
#define As(NAME) {#NAME,
#define please }}},



BNF rules {{
        
        
    As(E) read nt(E) x t(+) x nt(T)
            Or nt(T)
            please
    
    As(T) read nt(T) x t(*) x nt(F)
            Or nt(F)
            please
    
    As(F) read t('(') x nt(E) x t(')')
            Or t(int)
            please        
        
        
}};



#undef read
#undef x
#undef Or
#undef nt
#undef t
#undef As
#undef please

} // ! namespace tabgen

#endif // ! PARSER_TABLE_GENERATOR_H
