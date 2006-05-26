namespace lemur {
  namespace api {
    %nodefault ScoreAccumulator;
    
    class ScoreAccumulator {
    };
  }
  namespace retrieval 
  {
    class ArrayAccumulator : public lemur::api::ScoreAccumulator {
    public:

      ArrayAccumulator(int maxID);
    };
  }
}
