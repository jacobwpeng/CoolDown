#ifndef PAYLOAD_TYPE_H
#define PAYLOAD_TYPE_H

enum payload_t{
    PAYLOAD_LOGIN = 10000,
    PAYLOAD_LOGOUT = 10001,
    PAYLOAD_REQUEST_PEER = 10002,
    PAYLOAD_REPORT_PROGRESS = 10003,
    PAYLOAD_PUBLISH_RESOURCE = 10004,

    PAYLOAD_MESSAGE_REPLY = 20001,
    PAYLOAD_REQUEST_PEER_REPLAY = 20002,
};

#endif