#pragma once

#include <memory>
#include <string>
#include <iostream>

using namespace std;

class IRegExpr;

using RegExprShPtr = std::shared_ptr<IRegExpr>;

class IRegExpr {
    public:
    virtual void output() = 0;
};

class ConcLit : public IRegExpr {
    public:
    ConcLit(const string& concstr);
    void output() override {
        cout << concstr_ << endl;
    }

    private:
    string concstr_;
};

class Conc : public IRegExpr {
    public:
    Conc(const RegExprShPtr& l, const RegExprShPtr& r);
    void output() override {
        cout << "." << endl;
        l_.get()->output();
        r_.get()->output();
    }
    private:
    RegExprShPtr l_;
    RegExprShPtr r_;
};

class Or : public IRegExpr {
    public:
    Or(const RegExprShPtr& l, const RegExprShPtr& r);
    void output() override {
        cout << "|" << endl;
        l_.get()->output();
        r_.get()->output();
    }
    private:
    RegExprShPtr l_;
    RegExprShPtr r_;
};

class Star : public IRegExpr {
    public:
    Star(const RegExprShPtr& opd);
    void output() override {
        cout << "*" << endl;
        opd_.get()->output();
    }
    private:
    RegExprShPtr opd_;
};
