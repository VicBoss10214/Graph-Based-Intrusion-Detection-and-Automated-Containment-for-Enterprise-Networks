#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "Traversal.h"
#include <queue>
#include <vector>
#include <iomanip>

using namespace std;

class IncidentPriorityQueue {

public:

    struct Incident {

        int sourceNode;

        double severity;
        double assetValue;
        double confidence;

        double riskScore;

        bool operator<(
            const Incident& other
        ) const {

            return riskScore <
                   other.riskScore;
        }
    };


private:

    priority_queue<Incident> pq;


public:

    void addIncident(
        int sourceNode,
        double severity,
        double assetValue,
        double confidence) {

        Incident incident;

        incident.sourceNode =
            sourceNode;

        incident.severity =
            severity;

        incident.assetValue =
            assetValue;

        incident.confidence =
            confidence;

        incident.riskScore =
            severity *
            assetValue *
            confidence;

        pq.push(incident);
    }


    bool empty() const {
        return pq.empty();
    }


    Incident getNextIncident() {

        Incident result =
            pq.top();

        pq.pop();

        return result;
    }


    void printPriorityOrder(
        const Graph& graph) {

        priority_queue<Incident> copy =
            pq;

        cout << "\n========== INCIDENT PRIORITY ==========\n";

        int position = 1;

        while (!copy.empty()) {

            Incident incident =
                copy.top();

            copy.pop();

            cout << position++
                 << ". "
                 << graph.getNode(
                        incident.sourceNode
                    ).name
                 << " | Risk Score: "
                 << fixed
                 << setprecision(2)
                 << incident.riskScore
                 << '\n';
        }

        cout << "=======================================\n";
    }
};

#endif