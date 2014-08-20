#include "message.hpp"
#include "exception.hpp"

using namespace riemannpp;

message::message()
	: d_message(riemann_message_new())
{}

message::message(riemann_message_t* m)
	: d_message(m)
{}

message::message(message&& m) {
	*this = std::move(m);
}

message::message(query& q) {
	d_message.reset(riemann_message_new());
	set_query(q);
}

message::~message() {
	if (d_message) {
		riemann_message_free(d_message.release());
	}
}

message&
message::operator=(message&& m) {
	d_message = std::move(m.d_message);
	return (*this);
}

void
message::set_event(event& e) {
	riemann_event_t* evp = e.release();
	int result = riemann_message_set_events_n(d_message.get(), 1, &evp);
	if (-1 == result) {
		throw new internal_exception();
	}
}

message&
message::operator<<(event &e) {
	return (*this);
}

void
message::set_query(query& q) {
	int result = riemann_message_set_query(d_message.get(), q.release());
	if (-1 == result) {
		throw new internal_exception();
	}
}

message&
message::operator<<(query &q) {
	set_query(q);
	return (*this);
}

bool
message::get_ok() {
	if (d_message && d_message->has_ok) {
		return (d_message->ok != 0);
	} else {
		return false;
	}
}

std::string
message::get_error() {
	if (d_message && d_message->error) {
		return (d_message->error);
	} else {
		return "";
	}
}

std::vector<event>
message::get_events() {
	std::vector<event> result;
	if (d_message && d_message->n_events) {
		for (size_t i = 0; i < d_message->n_events; ++i) {
			result.push_back(event(d_message->events[i]));
		}
	}
	return result;
}

query
message::get_query() {
	if (d_message && d_message->query) {
		return query(d_message->query);
	} else {
		return query();
	}
}
