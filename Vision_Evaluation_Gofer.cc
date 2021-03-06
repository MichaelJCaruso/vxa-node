/*****  Vision_Evaluation_Gofer Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include <iostream>

/******************
 *****  Self  *****
 ******************/

#include "Vision_Evaluation_Gofer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_VEvaluatorClient.h"


/********************************************************
 ********************************************************
 *****                                              *****
 *****  Vision::Evaluation::Gofer::EvaluatorClient  *****
 *****                                              *****
 ********************************************************
 ********************************************************/

class Vision::Evaluation::Gofer::EvaluatorClient : public Vsa::VEvaluatorClient {
    DECLARE_CONCRETE_RTTLITE (EvaluatorClient, Vsa::VEvaluatorClient);

//  Construction
public:
    EvaluatorClient (Gofer *pGofer, export_t *pExport);

//  Destruction
private:
    ~EvaluatorClient () {
    }

//  Callbacks
protected:
    void OnError_(Vca::IError *pInterface, VString const &rMessage) override;
    void OnResult_(Vsa::IEvaluationResult *pInterface, VString const &rOutput) override;

//  State
private:
    Gofer::Reference const m_pGofer;
};


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vision::Evaluation::Gofer::EvaluatorClient::EvaluatorClient (
    Gofer *pGofer, export_t *pExport
) : m_pGofer (pGofer) {
    aggregate (pExport);
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vision::Evaluation::Gofer::EvaluatorClient::OnError_(Vca::IError *pInterface, VString const &rMessage) {
    m_pGofer->onError (pInterface, rMessage);
    BaseClass::OnError_(pInterface, rMessage);
}

void Vision::Evaluation::Gofer::EvaluatorClient::OnResult_(Vsa::IEvaluationResult *pInterface, VString const &rOutput) {
    Value const iResult (pInterface, rOutput);
    m_pGofer->setTo (iResult);
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vision::Evaluation::Gofer  *****
 *****                             *****
 ***************************************
 ***************************************/

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vision::Evaluation::Gofer::onNeed () {
    m_pEvaluator.materializeFor (this);
    m_iExpression.materializeFor (this);
    m_pExport.materializeFor (this);
    BaseClass::onNeed ();
}

void Vision::Evaluation::Gofer::onData () {
    EvaluatorClient::Reference const pEvaluatorClient (new EvaluatorClient (this, m_pExport));
    pEvaluatorClient->onQuery (m_pEvaluator, m_iExpression);
}
