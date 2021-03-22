/*
 * CryptoPlayer.cpp
 *
 */

#include "CryptoPlayer.h"
#include "Math/Setup.h"

void check_ssl_file(string filename)
{
    if (not ifstream(filename))
        throw runtime_error("Cannot access " + filename
                        + ". Have you set up SSL?\n"
                        "You can use `Scripts/setup-ssl.sh <nparties>`.");
}

void ssl_error(string side, string pronoun, string other, string server)
{
    cerr << side << "-side handshake with " << other
            << " failed. Make sure " << pronoun
            << " have the necessary certificate (" << PREP_DIR << server
            << ".pem in the default configuration),"
            << " and run `c_rehash <directory>` on its location." << endl
            << "Also make sure that it's still valid. Certificates generated "
            << "with `Scripts/setup-ssl.sh` expire after a month." << endl;
}

CryptoPlayer::CryptoPlayer(const Names& Nms, int id_base) :
        MultiPlayer<ssl_socket*>(Nms, id_base), plaintext_player(Nms, id_base),
        other_player(Nms, id_base + Nms.num_players()),
        ctx("P" + to_string(my_num()))
{
    sockets.resize(num_players());
    other_sockets.resize(num_players());
    senders.resize(num_players());
    receivers.resize(num_players());

    for (int i = 0; i < (int)sockets.size(); i++)
    {
        if (i == my_num())
        {
            sockets[i] = 0;
            senders[i] = 0;
            other_sockets[i] = 0;
            receivers[i] = 0;
            continue;
        }

        sockets[i] = new ssl_socket(io_service, ctx, plaintext_player.socket(i),
                "P" + to_string(i), "P" + to_string(my_num()), i < my_num());
        other_sockets[i] = new ssl_socket(io_service, ctx, other_player.socket(i),
                "P" + to_string(i), "P" + to_string(my_num()), i < my_num());

        senders[i] = new Sender<ssl_socket*>(i < my_num() ? sockets[i] : other_sockets[i]);
        receivers[i] = new Receiver<ssl_socket*>(i < my_num() ? other_sockets[i] : sockets[i]);
    }
}

CryptoPlayer::~CryptoPlayer()
{
    close_client_socket(plaintext_player.socket(my_num()));
    close_client_socket(other_player.socket(my_num()));
    plaintext_player.sockets.clear();
    other_player.sockets.clear();
    for (int i = 0; i < num_players(); i++)
    {
        delete sockets[i];
        delete other_sockets[i];
        delete senders[i];
        delete receivers[i];
    }
}

void CryptoPlayer::send_to_no_stats(int other, const octetStream& o) const
{
    senders[other]->request(o);
    senders[other]->wait(o);
}

void CryptoPlayer::receive_player_no_stats(int other, octetStream& o) const
{
    receivers[other]->request(o);
    receivers[other]->wait(o);
}

void CryptoPlayer::exchange_no_stats(int other, const octetStream& to_send,
        octetStream& to_receive) const
{
    if (&to_send == &to_receive)
    {
        MultiPlayer<ssl_socket*>::exchange_no_stats(other, to_send, to_receive);
    }
    else
    {
        senders[other]->request(to_send);
        receivers[other]->request(to_receive);
        senders[other]->wait(to_send);
        receivers[other]->wait(to_receive);
    }
}

void CryptoPlayer::pass_around_no_stats(const octetStream& to_send,
        octetStream& to_receive, int offset) const
{
    if (&to_send == &to_receive)
    {
        MultiPlayer<ssl_socket*>::pass_around_no_stats(to_send, to_receive, offset);
    }
    else
    {
#ifdef TIME_ROUNDS
        Timer recv_timer;
        TimeScope ts(recv_timer);
#endif
        senders[get_player(offset)]->request(to_send);
        receivers[get_player(-offset)]->request(to_receive);
        senders[get_player(offset)]->wait(to_send);
        receivers[get_player(-offset)]->wait(to_receive);
#ifdef TIME_ROUNDS
        cout << "Exchange time: " << recv_timer.elapsed() << " seconds to receive "
            << 1e-3 * to_receive.get_length() << " KB" << endl;
#endif
    }
}

template<>
void MultiPlayer<ssl_socket*>::setup_sockets(const vector<string>& names,
        const vector<int>& ports, int id_base, ServerSocket& server)
{
    (void)names, (void)ports, (void)id_base, (void)server;
}

void CryptoPlayer::send_receive_all_no_stats(const vector<vector<bool>>& channels,
        const vector<octetStream>& to_send,
        vector<octetStream>& to_receive) const
{
    to_receive.resize(num_players());
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        bool receive = channels[other][my_num()];
        if (channels[my_num()][other])
            this->senders[other]->request(to_send[other]);
        if (receive)
            this->receivers[other]->request(to_receive[other]);
    }
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        bool receive = channels[other][my_num()];
        if (channels[my_num()][other])
            this->senders[other]->wait(to_send[other]);
        if (receive)
            this->receivers[other]->wait(to_receive[other]);
    }
}

void CryptoPlayer::partial_broadcast(const vector<bool>& my_senders,
        const vector<bool>& my_receivers,
        vector<octetStream>& os) const
{
    TimeScope ts(comm_stats["Partial broadcasting"].add(os[my_num()]));
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        bool receive = my_senders[other];
        if (my_receivers[other])
        {
            this->senders[other]->request(os[my_num()]);
            sent += os[my_num()].get_length();
        }
        if (receive)
            this->receivers[other]->request(os[other]);
    }
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        bool receive = my_senders[other];
        if (my_receivers[other])
            this->senders[other]->wait(os[my_num()]);
        if (receive)
            this->receivers[other]->wait(os[other]);
    }
}

void CryptoPlayer::Broadcast_Receive_no_stats(vector<octetStream>& os) const
{
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        this->senders[other]->request(os[my_num()]);
        receivers[other]->request(os[other]);
    }

    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        this->senders[other]->wait(os[my_num()]);
        receivers[other]->wait(os[other]);
    }
}
