#ifndef __INEEDS_NOTIFIER_HPP__
# define __INEEDS_NOTIFIER_HPP__

class IWritableNotifier;

class INeedsNotifier {
public:
	virtual ~INeedsNotifier() {}

	virtual void	setNotifier(IWritableNotifier &notifier) = 0;
};

#endif /* __INEEDS_NOTIFIER_HPP__ */