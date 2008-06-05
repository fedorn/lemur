/*
 * Main.java
 *
 * Created on November 30, 2006, 9:25 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package ireval;

import ireval.RetrievalEvaluator.Document;
import ireval.RetrievalEvaluator.Judgment;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;

/**
 *
 * @author trevor
 */
public class Main {
    /**
     * Loads a TREC judgments file.
     *
     * @param filename The filename of the judgments file to load.
     * @return Maps from query numbers to lists of judgments for each query.
     */
    
    public static TreeMap< String, ArrayList<Judgment> > loadJudgments( String filename ) throws IOException, FileNotFoundException {
        // open file
        BufferedReader in = new BufferedReader(new FileReader( filename ));
        String line = null;
        TreeMap< String, ArrayList<Judgment> > judgments = new TreeMap< String, ArrayList<Judgment> >();
        String recentQuery = null;
        ArrayList<Judgment> recentJudgments = null;
        
        while( (line = in.readLine()) != null ) {
            // allow for multiple whitespace characters between fields
            String[] fields = line.split( "\\s+" );
            
            String number = fields[0];
            String unused = fields[1];
            String docno = fields[2];
            String judgment = fields[3];
            int jVal = 0;
            try {
                jVal = Integer.valueOf( judgment );
            } catch (NumberFormatException e) {
                jVal = (int)Math.round(Double.valueOf( judgment ));
            }
            
            Judgment j = new Judgment( docno, jVal );
            
            if( recentQuery == null || !recentQuery.equals( number ) ) {
                if( !judgments.containsKey( number ) ) {
                    judgments.put( number, new ArrayList<Judgment>() );
                }
                
                recentJudgments = judgments.get( number );
                recentQuery = number;
            }
            
            recentJudgments.add( j );
        }

        in.close();
        return judgments;
    }
    
    /**
     * Reads in a TREC ranking file.
     *
     * @param filename The filename of the ranking file.
     * @return A map from query numbers to document ranking lists.
     */

    public static TreeMap< String, ArrayList<Document> > loadRanking( String filename ) throws IOException, FileNotFoundException {
        // open file
        BufferedReader in = new BufferedReader(new FileReader( filename ));
        String line = null;
        TreeMap< String, ArrayList<Document> > ranking = new TreeMap< String, ArrayList<Document> >();
        ArrayList<Document> recentRanking = null;
        String recentQuery = null;
        
        while( (line = in.readLine()) != null ) {
            // allow for multiple whitespace characters between fields
            String[] fields = line.split( "\\s+" );
            
            // 1 Q0 WSJ880711-0086 39 -3.05948 Exp
                    
            String number = fields[0];
            String unused = fields[1];
            String docno = fields[2];
            String rank = fields[3];
            String score = fields[4];
            String runtag = fields[5];
            
            Document document = new Document( docno, Integer.valueOf( rank ), Double.valueOf( score ) );
            
            if( recentQuery == null || !recentQuery.equals( number ) ) {
                if( !ranking.containsKey( number ) ) {
                    ranking.put( number, new ArrayList<Document>() );
                }
                
                recentQuery = number;
                recentRanking = ranking.get( number );       
            }
            
            recentRanking.add( document );
        }
        
        in.close();
        return ranking;
    }

    /**
     * Creates a SetRetrievalEvaluator from data from loadRanking and loadJudgments.
     */
    
    public static SetRetrievalEvaluator create( TreeMap< String, ArrayList<Document> > allRankings, TreeMap< String, ArrayList<Judgment> > allJudgments ) {
        // Map query numbers into Integer to get proper sorting.
        TreeMap< Integer, RetrievalEvaluator > evaluators = new TreeMap<Integer, RetrievalEvaluator>();
        
        for( String query : allRankings.keySet() ) {
            ArrayList<Judgment> judgments = allJudgments.get( query );
            ArrayList<Document> ranking = allRankings.get( query );
            
            if( judgments == null || ranking == null ) {
                continue;
            }
            
            RetrievalEvaluator evaluator = new RetrievalEvaluator( query, ranking, judgments );
            evaluators.put( Integer.valueOf(query), evaluator );
        }
        
        return new SetRetrievalEvaluator( evaluators.values() );
    }
    
    /**
     * When run as a standalone application, this returns output 
     * very similar to that of trec_eval.  The first argument is 
     * the ranking file, and the second argument is the judgments
     * file, both in standard TREC format.
     */
    public static void singleEvaluation( SetRetrievalEvaluator setEvaluator ) {
        String formatString = "%2$-15s\t%1$s\t";
        
        // print trec_eval relational-style output
        for( RetrievalEvaluator evaluator : setEvaluator.getEvaluators() ) {
            String query = evaluator.queryName();
            
            // counts
            System.out.format( formatString + "%3$d\n",        query, "num_ret",     evaluator.retrievedDocuments().size() );
            System.out.format( formatString + "%3$d\n",        query, "num_rel",     evaluator.relevantDocuments().size() );
            System.out.format( formatString + "%3$d\n",        query, "num_rel_ret", evaluator.relevantRetrievedDocuments().size() );

            // aggregate measures
            System.out.format( formatString + "%3$6.4f\n",     query, "map",         evaluator.averagePrecision() );
            System.out.format( formatString + "%3$6.4f\n",     query, "ndcg",        evaluator.normalizedDiscountedCumulativeGain() );
            System.out.format( formatString + "%3$6.4f\n",     query, "ndcg15",      evaluator.normalizedDiscountedCumulativeGain( 15 ) );
            System.out.format( formatString + "%3$6.4f\n",     query, "R-prec",      evaluator.rPrecision() );
            System.out.format( formatString + "%3$6.4f\n",     query, "bpref",       evaluator.binaryPreference() );
            System.out.format( formatString + "%3$6.4f\n",     query, "recip_rank",  evaluator.reciprocalRank() );
            
            // precision at fixed points
            int[] fixedPoints = { 5, 10, 15, 20, 30, 100, 200, 500, 1000 };
            
            for( int i=0; i<fixedPoints.length; i++ ) {
                int point = fixedPoints[i];
                System.out.format( formatString + "%3$6.4f\n", query, "P" + point,   evaluator.precision( fixedPoints[i] ) );
            }
        }
        
        // print summary data
        System.out.format( formatString + "%3$d\n",      "all", "num_q",     setEvaluator.getEvaluators().size() );
        System.out.format( formatString + "%3$d\n",      "all", "num_ret",     setEvaluator.numberRetrieved() );
        System.out.format( formatString + "%3$d\n",      "all", "num_rel",     setEvaluator.numberRelevant() );
        System.out.format( formatString + "%3$d\n",      "all", "num_rel_ret", setEvaluator.numberRelevantRetrieved() );
        
        System.out.format( formatString + "%3$6.4f\n",   "all", "map",         setEvaluator.meanAveragePrecision() );
            System.out.format( formatString + "%3$6.4f\n",     "all", "gm_ap",         setEvaluator.geometricMeanAveragePrecision() );
        System.out.format( formatString + "%3$6.4f\n",   "all", "ndcg",        setEvaluator.meanNormalizedDiscountedCumulativeGain() );
        System.out.format( formatString + "%3$6.4f\n",   "all", "R-prec",      setEvaluator.meanRPrecision() );
        System.out.format( formatString + "%3$6.4f\n",   "all", "bpref",       setEvaluator.meanBinaryPreference() );
        System.out.format( formatString + "%3$6.4f\n",   "all", "recip_rank",  setEvaluator.meanReciprocalRank() );
        
        // precision at fixed points
        int[] fixedPoints = { 5, 10, 15, 20, 30, 100, 200, 500, 1000 };

        for( int i=0; i<fixedPoints.length; i++ ) {
            int point = fixedPoints[i];
            System.out.format( formatString + "%3$6.4f\n", "all", "P" + point,   setEvaluator.meanPrecision( fixedPoints[i] ) );
        }
    }

    /**
     * Compares two ranked lists with statistical tests on most major metrics.
     */

    
    public static void comparisonEvaluation( SetRetrievalEvaluator baseline, SetRetrievalEvaluator treatment ) {
        String[] metrics = { "averagePrecision", "ndcg", "bpref", "P10", "P20" };
        String formatString = "%1$-20s%2$-12s%3$6.4f\n";
        String integerFormatString = "%1$-20s%2$-12s%3$d\n";
        
        for( String metric : metrics ) {
            Map<String, Double> baselineMetric = baseline.evaluateAll( metric );
            Map<String, Double> treatmentMetric = treatment.evaluateAll( metric );
            
            SetRetrievalComparator comparator = new SetRetrievalComparator( baselineMetric, treatmentMetric );

            System.out.format( formatString, metric, "baseline", comparator.meanBaselineMetric() );
            System.out.format( formatString, metric, "treatment", comparator.meanTreatmentMetric() );
            
            System.out.format( integerFormatString, metric, "basebetter", comparator.countBaselineBetter() );
            System.out.format( integerFormatString, metric, "treatbetter", comparator.countTreatmentBetter() );
            System.out.format( integerFormatString, metric, "equal", comparator.countEqual() );
            
            System.out.format( formatString, metric, "ttest", comparator.pairedTTest() );
            System.out.format( formatString, metric, "randomized", comparator.randomizedTest() );
            System.out.format( formatString, metric, "signtest", comparator.signTest() );
        }
    }
    
    public static void usage( ) {
        System.err.println( "ireval: " );
        System.err.println( "   There are two ways to use this program.  First, you can evaluate a single ranking: " );
        System.err.println( "      java -jar ireval.jar TREC-Ranking-File TREC-Judgments-File" );
        System.err.println( "   or, you can use it to compare two rankings with statistical tests: " );
        System.err.println( "      java -jar ireval.jar TREC-Baseline-Ranking-File TREC-Improved-Ranking-File TREC-Judgments-File" );
        System.exit(-1);
    }
    
    public static void main( String[] args ) throws IOException {
        try {
            if( args.length == 3 ) {
                TreeMap< String, ArrayList<Document> > baselineRanking = loadRanking( args[0] );
                TreeMap< String, ArrayList<Document> > treatmentRanking = loadRanking( args[1] );
                TreeMap< String, ArrayList<Judgment> > judgments = loadJudgments( args[2] );

                SetRetrievalEvaluator baseline = create( baselineRanking, judgments );
                SetRetrievalEvaluator treatment = create( treatmentRanking, judgments );

                comparisonEvaluation( baseline, treatment );
            } else if( args.length == 2 ) {
                TreeMap< String, ArrayList<Document> > ranking = loadRanking( args[0] );
                TreeMap< String, ArrayList<Judgment> > judgments = loadJudgments( args[1] );

                SetRetrievalEvaluator setEvaluator = create( ranking, judgments );
                singleEvaluation( setEvaluator );
            } else {
                usage();
            }
        } catch( Exception e ) {
            e.printStackTrace();
            usage();
        }
    }
}
