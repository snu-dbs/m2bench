//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "../include/Cursor.h"

void Cursor::push_vector(const vector<json>& v) {
    // push all vector values to the queue
    for (auto & it : v) q.push(it);
}

Cursor::Cursor(Session* session) {
    auto firstData = session->fetch();
    push_vector(firstData);
    this->session = session;
}

bool Cursor::hasNext() {
    if (q.empty() && session->isDone()) return false;
    return true;
}

json Cursor::next() {
    if (!hasNext()) return "{}"_json;
    if (q.empty()) {        // TODO: duplicated
        auto data = session->fetch();
        push_vector(data);
    }

    auto ret = q.front();
    q.pop();
    return ret;
}

