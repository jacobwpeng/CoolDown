#ifndef CLIENT_H
#define CLIENT_H

#include "error_code.h"
#include "local_sock_manager.h"
#include "net_task_manager.h"
#include <vector>
#include <string>
#include <Poco/Util/Application.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Types.h>
#include <google/protobuf/message.h>
using google::protobuf::Message;

using std::vector;
using std::string;
using Poco::Util::Application;
using Poco::Path;
using Poco::File;
using Poco::Int32;
using Poco::Int64;

namespace CoolDown{
    namespace Client{

            class CoolClient : public Application{
                public:
                    CoolClient(int argc, char* argv[]);
                    const static unsigned int TRACKER_PORT = 9977;
                    typedef vector<string> ClientIdCollection;
                    typedef vector<File> FileList;
                    typedef LocalSockManager::LocalSockManagerPtr LocalSockManagerPtr;
                    typedef int make_torrent_progress_callback_t;
                    void initialize(Application& self);
                    void uninitialize();
                    int main(const vector<string>& args);

                    NetTaskManager& download_manager();
                    NetTaskManager& upload_manager();

                    retcode_t login_tracker(const string& tracker_address, int port = TRACKER_PORT);
                    retcode_t logout_tracker(const string& tracker_address, int port = TRACKER_PORT);
                    retcode_t publish_resource_to_tracker(const string& tracker_address, const string& fileid);
                    retcode_t report_progress(const string& tracker_address, const string& fileid, int percentage);
                    retcode_t request_clients(const string& tracker_address, const string& fileid, int currentPercentage, 
                                          int needCount, const ClientIdCollection& clientids);
                    //retcode_t make_torrent(const Path& path, TorrentInfo&);
                    retcode_t make_torrent(const Path& path, const Path& torrent_file_path, 
                            Int32 chunk_size, Int32 type, const string& tracker_address);

                    string clientid() const;
                    string current_time() const;

                    void set_make_torrent_progress_callback(make_torrent_progress_callback_t callback);

                private:

                    template<typename ReplyMessageType>
                    retcode_t handle_reply_message(LocalSockManager::SockPtr& sock, 
                    const Message& msg, int payload_type, SharedPtr<ReplyMessageType>* out);

                    void list_dir_recursive(const File& file, FileList* pList);

                    bool init_error_;
                    string clientid_;
                    LocalSockManagerPtr sockManager_;
                    NetTaskManager downloadManager_;
                    NetTaskManager uploadManager_;

                    make_torrent_progress_callback_t make_torrent_progress_callback_;

            };
    }
}

#endif
