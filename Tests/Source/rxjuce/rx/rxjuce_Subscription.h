/*
  ==============================================================================

    rxjuce_Subscription.h
    Created: 27 Apr 2017 7:09:19am
    Author:  Martin Finke

  ==============================================================================
*/

#pragma once

#include "rxjuce_Prefix.h"

RXJUCE_NAMESPACE_BEGIN

/**
	Manages the lifetime of a subscription to an Observable.
 
	@see Observable::subscribe
 */
class Subscription
{
public:
	
	///@{
	/**
		Transfers ownership from an existing Subscription.
	 */
	Subscription(Subscription&&) = default;
	Subscription& operator=(Subscription&&) = default;
	///@}
	
	/**
		Unsubuscribes from the Observable.
	 */
	void unsubscribe() const;
	
	/**
		Returns true iff the Subscription is still subscribed to the source Observable.
	 */
	bool isSubscribed() const;
	
private:
	struct Impl;
	const std::shared_ptr<Impl> impl;
	
	friend class Observable;
	explicit Subscription(const std::shared_ptr<Impl>&);
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Subscription)
};


/**
	A Subscription that automatically unsubscribes when it is destroyed.
 
	You can use this to prevent onNext from getting called after one of your objects is destroyed.
 
	For example:
		
		class MyClass
		{
		public:
			MyClass(Observable input)
			: subscription(input.subscribe([this](var newValue) { this->doSomething(newValue); })) {}
 
		private:
			const ScopedSubscription subscription;
		};
 
	In this example, `subscription` is destroyed when an instance of `MyClass` is destroyed. So the instance will stop receiving items from `input`.
 */
class ScopedSubscription : public Subscription
{
public:
	///@{
	/**
		Transfers ownership from an existing Subscription.
	 */
	ScopedSubscription(Subscription&& subscription);
	ScopedSubscription(ScopedSubscription&&) = default;
	ScopedSubscription& operator=(ScopedSubscription&&) = default;
	///@}
	
	/** Calls unsubscribe(). */
	~ScopedSubscription();
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedSubscription)
};

RXJUCE_NAMESPACE_END
