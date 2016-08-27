#include <list>
#include <map>
#include <utility>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

map<string, string> BC_OR(map<string, struct clauses> KB, string goal, map<string, string> theta);
map<string, string> BC_AND(map<string, struct clauses> KB, string goal, map<string, string> theta);
map<string, int> noOfRules;
vector<string> visited;

struct clauses
{
	vector<string> positive;
	vector<string> negative;
	vector<string> positiveConclusion;
	vector<string> negativeConclusion;
};

bool isSentence(string clause) {

	size_t found = clause.find("=>");
	if (found == string::npos)
		return false;
	return true;
}

bool isNegativeLiteral(string clause) {

	size_t found = clause.find("~");
	if (found == string::npos)
		return false;
	return true;
}

string getLHS(string clause) {

	size_t found = clause.find("=>");
	return clause.substr(0, found - 1);
}

string getRHS(string clause) {
	size_t found = clause.find("=>");
	return clause.substr(found + 3, clause.size() - (found + 2));
}

string firstConjunct(string clause) {
	size_t found = clause.find("^");
	if (found == string::npos)
		return clause;
	return clause.substr(0, found - 1);
}

string restConjunct(string clause) {
	size_t found = clause.find("^");
	if (found == string::npos)
		return "";
	return clause.substr(found + 2, clause.size() - (found + 2));
}

string getKey(string clause) {
	size_t found = clause.find("(");
	return clause.substr(0, found);
}

vector<string> fetchRulesFromKB(map<string, struct clauses> KB, string goal) {

	string goalKey = getKey(goal);
	bool negative = isNegativeLiteral(goal);
	vector<string> temp;

	if (negative)
		goalKey = goalKey.erase(0, 1);

	if (KB.find(goalKey) != KB.end()) { //if it is present in the KB
		struct clauses goalClause = KB.at(goalKey);

		if (negative) {
			temp = goalClause.negativeConclusion;
			temp.insert(temp.end(), goalClause.negative.begin(), goalClause.negative.end());

		}
		else {
			temp = goalClause.positiveConclusion;
			temp.insert(temp.end(), goalClause.positive.begin(), goalClause.positive.end());
		}

		return temp;
	}

}

map<string, string> unify(string rhsClause, string goal, map<string, string> theta) {

	map<string, string> updatedTheta;
	if (rhsClause.compare(goal) == 0) {
		cout << "rhs n goal are same";
		return updatedTheta;
	}

	else {

		size_t foundInClause;
		size_t foundInGoal;
		size_t found = rhsClause.find("(");
		rhsClause.erase(0, found + 1);
		rhsClause.erase(rhsClause.size() - 1, 1);
		goal.erase(goal.size() - 1, 1);
		goal.erase(0, found + 1);
		foundInClause = rhsClause.find(",");
		foundInGoal = goal.find(",");

		while (!rhsClause.empty() && !goal.empty() && foundInClause != string::npos && foundInGoal != string::npos) {

			if (rhsClause.substr(0, foundInClause).size() == 1 && islower(rhsClause.substr(0, foundInClause)[0]) && (goal.substr(0, foundInGoal).size() != 1 || (goal.substr(0, foundInGoal).size() == 1 && !islower(goal.substr(0, foundInGoal)[0])))) {
				cout << "Hoon yesss1";
				//if (updatedTheta.find(rhsClause.substr(0, foundInClause)) != updatedTheta.end())
				//updatedTheta.erase(rhsClause.substr(0, foundInClause));
				if (updatedTheta.find(rhsClause.substr(0, foundInClause)) != updatedTheta.end()) {
					std::map<string, string>::iterator it = updatedTheta.find(rhsClause.substr(0, foundInClause));
					if (it->second != goal.substr(0, foundInGoal))
					{
						updatedTheta.insert(std::pair<string, string>("failure", "failure"));
						return updatedTheta;
					}
				}
				else {
					updatedTheta.insert(std::pair<string, string>(rhsClause.substr(0, foundInClause), goal.substr(0, foundInGoal)));
				}

			}
			else if (goal.substr(0, foundInGoal).size() == 1 && islower(goal.substr(0, foundInGoal)[0]) && (rhsClause.substr(0, foundInClause).size() != 1 || (rhsClause.substr(0, foundInClause).size() == 1 && !islower(rhsClause.substr(0, foundInClause)[0])))) {
				cout << "Hoon yesss2";

				if (updatedTheta.find(goal.substr(0, foundInGoal)) != updatedTheta.end()) {
					std::map<string, string>::iterator it = updatedTheta.find(goal.substr(0, foundInGoal));
					if (it->second != rhsClause.substr(0, foundInClause))
					{
						updatedTheta.insert(std::pair<string, string>("failure", "failure"));
						return updatedTheta;
					}
				}
				else {
					updatedTheta.insert(std::pair<string, string>(goal.substr(0, foundInGoal), rhsClause.substr(0, foundInClause)));
				}

				//if (updatedTheta.find(goal.substr(0, foundInGoal)) != updatedTheta.end())
				//updatedTheta.erase(goal.substr(0, foundInGoal));

			}
			else
			{
				if (rhsClause.substr(0, foundInClause).compare(goal.substr(0, foundInGoal)) != 0 && !(goal.substr(0, foundInGoal).size() == 1 && rhsClause.substr(0, foundInClause).size() == 1 && islower(goal.substr(0, foundInGoal)[0]) && rhsClause.substr(0, foundInClause)[0])) {
					updatedTheta.insert(std::pair<string, string>("failure", "failure"));
					return updatedTheta;
				}
			}
			rhsClause.erase(0, foundInClause + 1);
			goal.erase(0, foundInGoal + 1);
			foundInClause = rhsClause.find(",");
			foundInGoal = goal.find(",");
		}

		cout << "In unify last one" << rhsClause << " " << goal;
		if (rhsClause.size() == 1 && islower(rhsClause[0]) && (goal.size() != 1 || (goal.size() == 1 && !islower(goal[0])))) {
			//if (updatedTheta.find(rhsClause) != updatedTheta.end())
			//updatedTheta.erase(rhsClause);
			if (updatedTheta.find(rhsClause) != updatedTheta.end()) {
				std::map<string, string>::iterator it = updatedTheta.find(rhsClause);
				if (it->second != goal)
				{
					updatedTheta.insert(std::pair<string, string>("failure", "failure"));
					return updatedTheta;
				}
			}
			else
				updatedTheta.insert(std::pair<string, string>(rhsClause, goal));
		}
		else if (goal.size() == 1 && islower(goal[0]) && (rhsClause.size() != 1 || (rhsClause.size() == 1 && !islower(rhsClause[0])))) {
			//if (updatedTheta.find(goal) != updatedTheta.end())
			//updatedTheta.erase(goal);

			if (updatedTheta.find(goal) != updatedTheta.end()) {
				std::map<string, string>::iterator it = updatedTheta.find(goal);
				if (it->second != rhsClause)
				{
					updatedTheta.insert(std::pair<string, string>("failure", "failure"));
					return updatedTheta;
				}
			}
			else
				updatedTheta.insert(std::pair<string, string>(goal, rhsClause));
		}
		else
		{
			if (rhsClause.compare(goal) != 0 && !(goal.size() == 1 && rhsClause.size() == 1 && islower(goal[0]) && rhsClause[0])) {
				updatedTheta.insert(std::pair<string, string>("failure", "failure"));
				return updatedTheta;
			}
		}
		cout << "\nIn unify function\n";
		for (std::map<string, string>::iterator it = updatedTheta.begin(); it != updatedTheta.end(); ++it)
			cout << it->first << " " << it->second << "\n";
		return updatedTheta;
	}

}

string substitute(string clause, map<string, string> theta) {

	string copyClause = clause;
	cout << clause;
	string select;
	string replaceString;
	if (!theta.empty()) {
		size_t found = copyClause.find("(");
		size_t first = found;
		copyClause.erase(0, found + 1);
		copyClause.erase(copyClause.size() - 1, 1);
		found = copyClause.find(",");
		int last = found;
		const char *s;

		while (found != string::npos && !copyClause.empty()) {

			select = copyClause.substr(0, found);
			s = select.c_str();
			cout << "select " << select;
			if (theta.find(select) != theta.end()) {
				replaceString = theta.at(select);
				cout << "replaceString " << replaceString;
				size_t selectFound = clause.find(s, first, select.size());
				clause.replace(selectFound, select.size(), replaceString);
				cout << "After replace " << clause;
				first = first + replaceString.size();
			}
			else {
				first = first + select.size();
			}

			copyClause.erase(0, found + 1);
			found = copyClause.find(",");
		}

		select = copyClause;
		s = select.c_str();
		cout << "Select " << select;
		if (theta.find(select) != theta.end()) {
			replaceString = theta.at(select);
			cout << "replaceString " << replaceString;
			size_t selectFound = clause.find(s, first, select.size());
			clause.replace(selectFound, select.size(), replaceString);
		}
		cout << " substitute clause " << clause;
		return clause;
	}
	else
		return clause;
}


bool containsConstants(string clause) {

	cout << "check const";
	size_t found = clause.find("(");
	clause.erase(0, found + 1);
	clause.erase(clause.size() - 1, 1);
	found = clause.find(",");
	while (found != string::npos) {
		cout << "in while " << clause;
		if (!(islower(clause.substr(0, found)[0]) && clause.substr(0, found).size() == 1)) {
			cout << "FDVEFDV\n" << clause.substr(0, found)[0] << islower(clause.substr(0, found)[0]) << clause.substr(0, found).size();
			return true;
		}

		clause.erase(0, found + 1);
		found = clause.find(",");
	}


	if (!(islower(clause[0]) && clause.size() == 1)) {
		cout << clause;
		return true;
	}

	return false;
}

vector<string>::iterator findVector(vector<string>::iterator first, vector<string>::iterator last, string temp)
{
	while (first != last) {
		if (*first == temp) return first;
		++first;
	}
	return last;
}


bool checkForLoop(string goal) {

	string temp;
	std::vector<string>::iterator it;
	if (containsConstants(goal))
		temp = goal;
	else
		temp = getKey(goal);

	cout << "Ouch";
	it = findVector(visited.begin(), visited.end(), temp);
	//it = findVector(visited, temp);
	if (it != visited.end()) {
		cout << "The clause which is already in the map " << goal;
		return true;
	}
	return false;
}


map<string, string> variableMapCreator(string rhsClause, string goal) {


	map<string, string> variableMap;
	size_t found = goal.find("(");
	goal.erase(0, found + 1);
	goal.erase(goal.size() - 1, 1);
	rhsClause.erase(0, found + 1);
	rhsClause.erase(rhsClause.size() - 1, 1);
	found = goal.find(",");
	cout << "before while";
	while (found != string::npos) {
		cout << "in while " << goal;
		if (islower(goal.substr(0, found)[0]) && goal.substr(0, found).size() == 1 && islower(rhsClause.substr(0, found)[0]) && rhsClause.substr(0, found).size() == 1) {
			variableMap.insert(std::pair<string, string>(goal.substr(0, found), rhsClause.substr(0, found)));
		}

		goal.erase(0, found + 1);
		rhsClause.erase(0, found + 1);
		found = goal.find(",");
	}

	if (islower(goal[0]) && goal.size() == 1 && islower(rhsClause[0]) && rhsClause.size() == 1) {

		variableMap.insert(std::pair<string, string>(goal, rhsClause));
	}
	cout << "Before returning";
	return variableMap;
}

map<string, string> combineTwoThetas(map<string, string> theta, map<string, string> tempTheta, string goal, map<string, string> variableMap) {

	vector<string> variable;
	size_t found = goal.find("(");
	goal.erase(0, found + 1);
	goal.erase(goal.size() - 1, 1);
	found = goal.find(",");
	while (found != string::npos) {
		cout << "in while " << goal;
		if (islower(goal.substr(0, found)[0]) && goal.substr(0, found).size() == 1) {
			variable.insert(variable.begin(), goal.substr(0, found));
		}

		goal.erase(0, found + 1);
		found = goal.find(",");
	}


	if (islower(goal[0]) && goal.size() == 1) {
		cout << goal;
		variable.insert(variable.begin(), goal);
	}

	for (std::map<string, string>::iterator it = tempTheta.begin(); it != tempTheta.end(); ++it) {
		std::vector<string>::iterator i;
		std::map<string, string>::iterator mapI;


		if (variableMap.find(it->first) != variableMap.end()) {
			mapI = variableMap.find(it->first);
			i = findVector(variable.begin(), variable.end(), mapI->second);
			//i = findVector(variable, mapI->second);
			if (i != variable.end()) {
				if (theta.find(mapI->second) == theta.end()) {
					theta.insert(std::pair<string, string>(mapI->second, it->second));
				}
			}
		}
		else {
			i = findVector(variable.begin(), variable.end(), it->first);
			//i = findVector(variable, it->first);
			if (i != variable.end()) {
				if (theta.find(it->first) == theta.end()) {
					theta.insert(std::pair<string, string>(it->first, it->second));
				}
			}
		}

	}

	return theta;
}

map<string, string> sameVariables(map<string, string> variableMap, map<string, string> theta) {

	cout << "In same var funvct ";
	map<string, string> tempMap = variableMap;
	string variables;
	int count = 0;

	for (std::map<string, string>::iterator it = tempMap.begin(); it != tempMap.end(); ++it) {
		variables = it->first;
		std::map<string, string>::iterator temp1 = it;
		cout << variables << " ";
		if (++temp1 != tempMap.end()) {
			for (std::map<string, string>::iterator temp = temp1; temp != tempMap.end(); ++temp) {
				if (it->second == temp->second) {
					variables = variables + "," + temp->first;

				}
			}

		}
		if (!variables.empty()) {
			if (variables.find(",") != string::npos) {
				count++;
				string x = "same";
				theta.insert(std::pair<string, string>(x.append(to_string(count)), variables));
			}
		}


	}
	return theta;
}

bool thetaIsNotValid(map<string, string> theta, map<string, string> tempTheta) {
	cout << "Ey";
	for (std::map<string, string>::iterator it = theta.begin(); it != theta.end(); ++it) {
		if (it->first.find("same") != string::npos) {

			std::map<string, string>::iterator ite;
			ite = tempTheta.find(it->second.substr(0, 1));
			if (ite != tempTheta.end()) {
				string value = ite->second;
				cout << "Value of that in temptheta" << value;
				it->second.erase(0, 2);

				while (!it->second.empty()) {
					ite = tempTheta.find(it->second.substr(0, 1));
					if (ite != tempTheta.end()) {
						if (value != tempTheta.at(it->second.substr(0, 1)))
							return true;
					}

					it->second.erase(0, 2);
				}
			}

		}
	}

	return false;
}

map<string, string> BC_OR(map<string, struct clauses> KB, string goal, map<string, string> theta) {

	//fetch from KB
	string lhsClause;
	string rhsClause;
	bool orValue = false;
	map<string, string> tempTheta;
	vector<string> tempVector;
	map<string, string> variableMap;
	int start = 0;
	if (!checkForLoop(goal)) {
		vector<string> KBrules = fetchRulesFromKB(KB, goal);

		string goalKey = getKey(goal);
		bool negative = isNegativeLiteral(goal);
		//if (negative)
		//goalKey = goalKey.erase(0, 1);


		string selector;
		if (containsConstants(goal))
			selector = goal;
		else
			selector = goalKey;
		cout << "Selector " << selector;
		if (noOfRules.find(selector) == noOfRules.end()) {
			noOfRules.insert(std::pair<string, int>(selector, KBrules.size()));
			start = 0;
			cout << "Number of rules " << noOfRules.at(selector);
		}
		else {
			start = KBrules.size() - noOfRules.at(selector);
		}


		visited.insert(visited.begin(), selector);
		cout << "Visited vector outside OR";
		for (std::vector<string>::iterator it = visited.begin(); it != visited.end(); ++it)
			std::cout << *it << ' ';
		cout << "\n ";

		cout << "\nNo of rules in OR\n";
		for (std::map<string, int>::iterator it = noOfRules.begin(); it != noOfRules.end(); ++it)
			cout << it->first << " " << it->second << "\n";

		cout << "\nPrint KB rules vector \n";
		for (std::vector<string>::iterator it = KBrules.begin(); it != KBrules.end(); ++it)
			std::cout << *it << ' ';
		cout << "\n";

		if (!KBrules.empty()) {
			for (std::vector<string>::iterator it = KBrules.begin() + start; it != KBrules.end(); ++it) {
				if (isSentence(*it)) {

					lhsClause = getLHS(*it);
					rhsClause = getRHS(*it);
					cout << "\nSentence" << *it << "\nLHS " << lhsClause << " RHS " << rhsClause;

				}
				else {
					lhsClause = "";
					rhsClause = *it;
					cout << "\nNot a sentence " << rhsClause << " and goal is " << goal << " and lhs is " << lhsClause;
				}
				cout << "Theta";

				cout << "Baaa pa";

				//for (std::map<string, string>::iterator it = theta.begin(); it != theta.end(); ++it)
				//cout << it->first << " " << it->second << "\n";

				variableMap = variableMapCreator(rhsClause, goal);
				cout << rhsClause << goal;
				tempTheta = BC_AND(KB, lhsClause, unify(rhsClause, goal, theta));

				theta = sameVariables(variableMap, theta);

				cout << "theta map";
				for (std::map<string, string>::iterator it = theta.begin(); it != theta.end(); ++it)
					cout << it->first << " " << it->second << "\n";


				if (thetaIsNotValid(theta, tempTheta)) {
					cout << "Theta isnt VALID";
					tempTheta.insert(std::pair<string, string>("failure", "failure"));
				}


				if (tempTheta.find("failure") == tempTheta.end()) {
					cout << "No failure";
					tempTheta = combineTwoThetas(theta, tempTheta, goal, variableMap);

					int changeInNoOfRules = noOfRules.at(selector);
					changeInNoOfRules--;
					if (changeInNoOfRules != 0) {
						std::map<string, int>::iterator ruleNumber = noOfRules.find(selector);
						ruleNumber->second = changeInNoOfRules;
					}
					else {
						noOfRules.erase(selector);
					}
					cout << "Visited vector before erase" << selector;
					for (std::vector<string>::iterator it = visited.begin(); it != visited.end(); ++it)
						std::cout << *it << ' ';
					cout << "\n ";
					std::vector<string>::iterator it;
					it = findVector(visited.begin(), visited.end(), selector);
					//it = findVector(visited, selector);
					visited.erase(it);
					cout << "Visited vector after erase";
					for (std::vector<string>::iterator it = visited.begin(); it != visited.end(); ++it)
						std::cout << *it << ' ';
					cout << "\n ";
					return tempTheta;
				}

				//}

				int changeInNoOfRules = noOfRules.at(selector);
				changeInNoOfRules--;
				if (changeInNoOfRules != 0) {
					std::map<string, int>::iterator ruleNumber = noOfRules.find(selector);
					ruleNumber->second = changeInNoOfRules;
				}
				else {
					noOfRules.erase(selector);
				}
			}

			std::vector<string>::iterator it;
			it = findVector(visited.begin(), visited.end(), selector);
			//it = findVector(visited, selector);
			visited.erase(it);

		}
	}

	tempTheta.insert(std::pair<string, string>("failure", "failure"));
	cout << "gothithu";
	return tempTheta;

}

map<string, string> BC_AND(map<string, struct clauses> KB, string goal, map<string, string> theta) {

	string first;
	string rest;
	map<string, string> tempTheta1;
	map<string, string> tempTheta2;
	string goalKey;
	string selector;

	//if theta is failure
	cout << "And funct";
	if (theta.find("failure") != theta.end()) {
		cout << "Enters failure";
		return theta;
	}
	else if (goal.empty()) {
		cout << "Goal empty";
		return theta;
	}
	else {

		first = firstConjunct(goal);
		cout << "first " << first;
		rest = restConjunct(goal);
		cout << "Rest" << rest << "aythu" << "First is" << first;

		goalKey = getKey(first);
		if (containsConstants(substitute(first, theta)))
			selector = substitute(first, theta);
		else
			selector = goalKey;

		cout << "SELECTOR IS" << selector;

		cout << " KEY " << goalKey << "No of rules ";

		cout << "\nNo of rules in AND \n";
		for (std::map<string, int>::iterator it = noOfRules.begin(); it != noOfRules.end(); ++it)
			cout << it->first << " " << it->second << "\n";

		do {

			cout << "\nPrint theta\n";
			for (std::map<string, string>::iterator it = theta.begin(); it != theta.end(); ++it)
				cout << it->first << " " << it->second << "\n";
			tempTheta1 = BC_OR(KB, substitute(first, theta), theta);
			if (checkForLoop(substitute(first, theta)))
				return tempTheta1;

			cout << "\nPrint theta1\n";
			for (std::map<string, string>::iterator it = tempTheta1.begin(); it != tempTheta1.end(); ++it)
				cout << it->first << " " << it->second << "\n";
			if (rest.empty())
				cout << " Baare";
			tempTheta2 = BC_AND(KB, rest, tempTheta1);

			cout << "Selector" << selector;
			if (tempTheta2.find("failure") == tempTheta2.end()) {
				return tempTheta2;
			}

			//cout << "No of rules = " << noOfRules.at(goalKey);
		} while (noOfRules.find(selector) != noOfRules.end());
		//while (noOfRules.at(goalKey) != 0);


		return tempTheta2;
	}

}

map<string, string> BC_Ask(map<string, struct clauses> KB, string goal, map<string, string> theta) {

	map<string, string> finalTheta;
	finalTheta = BC_OR(KB, goal, theta);
	return finalTheta;
}

int main(int argc, char *argv[])
{
	ifstream inFile(argv[argc - 1]);
	//ifstream inFile("input.txt");
	ofstream outFile("output.txt");
	map<string, struct clauses> KB;
	string clause;
	string line;
	string lhs;
	string rhs;
	string key;
	struct clauses temp;
	map<string, string> theta;
	map<string, string> finalTheta;
	bool finalBool = false;
	int noOfQueries;
	getline(inFile, line);
	istringstream iss1(line);
	iss1 >> noOfQueries;
	string *queries;
	queries = new string[noOfQueries];
	for (int i = 0; i < noOfQueries; i++) {
		getline(inFile, line);
		queries[i] = line;
		cout << queries[i] << "\n";
	}

	int noOfClauses;
	getline(inFile, line);
	istringstream iss2(line);
	iss2 >> noOfClauses;
	for (int i = 1; i <= noOfClauses; i++) {

		temp.positiveConclusion.clear();
		temp.negativeConclusion.clear();
		temp.negative.clear();
		temp.positive.clear();

		getline(inFile, clause);

		if (isSentence(clause)) {
			rhs = getRHS(clause);
			key = getKey(rhs);
			//			cout << clause << " " << rhs << " " << key << "\n";

			bool negative = isNegativeLiteral(key);
			if (negative) {
				key = key.erase(0, 1); //erasing tilde ~
			}

			if (KB.find(key) == KB.end()) {
				if (negative)
					temp.negativeConclusion.insert(temp.negativeConclusion.begin(), clause);
				else
					temp.positiveConclusion.insert(temp.positiveConclusion.begin(), clause);
				KB.insert(std::pair<string, struct clauses>(key, temp));
			}
			else {
				temp = KB.at(key);
				if (negative)
					temp.negativeConclusion.insert(temp.negativeConclusion.end(), clause);
				else
					temp.positiveConclusion.insert(temp.positiveConclusion.end(), clause);
				std::map<string, struct clauses>::iterator it = KB.find(key);
				it->second = temp;
			}
		}
		else {
			// if it is a negative or positive literal
			key = getKey(clause);
			//cout << clause << " " << key << "\n";
			bool negative = isNegativeLiteral(clause);

			if (negative) {
				key = key.erase(0, 1); //erasing tilde ~
			}

			if (KB.find(key) == KB.end()) {
				if (negative)
					temp.negative.insert(temp.negative.begin(), clause);
				else
					temp.positive.insert(temp.positive.begin(), clause);

				KB.insert(std::pair<string, struct clauses>(key, temp));
			}
			else {
				temp = KB.at(key);
				//temp = KB.find(key)->second;
				if (negative)
					temp.negative.insert(temp.negative.end(), clause);
				else
					temp.positive.insert(temp.positive.end(), clause);

				std::map<string, struct clauses>::iterator it = KB.find(key);
				it->second = temp;
			}
		}
	}

	for (int i = 0; i < noOfQueries; i++) {

		finalTheta = BC_Ask(KB, queries[i], theta);
		if (finalTheta.find("failure") == finalTheta.end())
			outFile << "TRUE\n";
		else
			outFile << "FALSE\n";
		theta.clear();
		visited.clear();
		noOfRules.clear();


	}

}