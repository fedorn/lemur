// 
// Warning: This file is automatically generated
// 

#ifndef INDRI_WALKER_HPP
#define INDRI_WALKER_HPP

namespace indri { 
  namespace lang { 
    class Walker { 
    public:
      virtual ~Walker();
      virtual void defaultBefore( class Node* n );
      virtual void defaultAfter( class Node* n );

      virtual void before( class IndexTerm* n );
      virtual void after( class IndexTerm* n );
      virtual void before( class Field* n );
      virtual void after( class Field* n );
      virtual void before( class ExtentInside* n );
      virtual void after( class ExtentInside* n );
      virtual void before( class ExtentOr* n );
      virtual void after( class ExtentOr* n );
      virtual void before( class ExtentAnd* n );
      virtual void after( class ExtentAnd* n );
      virtual void before( class UWNode* n );
      virtual void after( class UWNode* n );
      virtual void before( class ODNode* n );
      virtual void after( class ODNode* n );
      virtual void before( class FilReqNode* n );
      virtual void after( class FilReqNode* n );
      virtual void before( class FilRejNode* n );
      virtual void after( class FilRejNode* n );
      virtual void before( class FieldLessNode* n );
      virtual void after( class FieldLessNode* n );
      virtual void before( class FieldGreaterNode* n );
      virtual void after( class FieldGreaterNode* n );
      virtual void before( class FieldBetweenNode* n );
      virtual void after( class FieldBetweenNode* n );
      virtual void before( class RawScorerNode* n );
      virtual void after( class RawScorerNode* n );
      virtual void before( class TermFrequencyScorerNode* n );
      virtual void after( class TermFrequencyScorerNode* n );
      virtual void before( class CachedFrequencyScorerNode* n );
      virtual void after( class CachedFrequencyScorerNode* n );
      virtual void before( class PriorNode* n );
      virtual void after( class PriorNode* n );
      virtual void before( class OrNode* n );
      virtual void after( class OrNode* n );
      virtual void before( class NotNode* n );
      virtual void after( class NotNode* n );
      virtual void before( class MaxNode* n );
      virtual void after( class MaxNode* n );
      virtual void before( class CombineNode* n );
      virtual void after( class CombineNode* n );
      virtual void before( class WAndNode* n );
      virtual void after( class WAndNode* n );
      virtual void before( class WSumNode* n );
      virtual void after( class WSumNode* n );
      virtual void before( class WeightNode* n );
      virtual void after( class WeightNode* n );
      virtual void before( class ExtentRestriction* n );
      virtual void after( class ExtentRestriction* n );
      virtual void before( class FilterNode* n );
      virtual void after( class FilterNode* n );
      virtual void before( class ContextCounterNode* n );
      virtual void after( class ContextCounterNode* n );
      virtual void before( class ScoreAccumulatorNode* n );
      virtual void after( class ScoreAccumulatorNode* n );
      virtual void before( class AnnotatorNode* n );
      virtual void after( class AnnotatorNode* n );
   };
 };
};

#endif // INDRI_WALKER_HPP

