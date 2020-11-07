<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;
use App\AlianceRequest;
use App\Aliance;
use App\newsModel;

class AlianceController extends Controller
{
 
    public function showAliances()
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 4){
            return redirect('no-permission');
        }
        $notShow = DB::select("SELECT a.ID_Familie AS PozvankaOd, a.JmenoFamilie AS JmenoOd, b.ID_Familie AS PozvankaPro, b.JmenoFamilie AS JmenoPro
				FROM PozvankaDoAliance AS c
				INNER JOIN Familie as a
				    ON c.ID_Familie1 = a.ID_Familie
				INNER JOIN Familie as b
				    ON c.ID_Familie2 = b.ID_Familie", [1]);

        $familie = DB::select("SELECT JmenoFamilie, ID_Familie FROM Familie WHERE ID_Dona IS NOT NULL AND ID_Familie != ".$_SESSION['familia']);
        $isInAliance = DB::select("SELECT * FROM Aliance WHERE Aliance1 = ".$_SESSION['familia']." OR Aliance2 = ".$_SESSION['familia']);

        foreach ($isInAliance as $aliance) {
        	if($aliance->Aliance1 == $_SESSION['familia'])
        	{
        		$i = 0;
        		foreach ($familie as $familia) {
        			if($familia->ID_Familie == $aliance->Aliance2)
        			{
        				unset($familie[$i]);
        			}
        			$i++;
        		}
        	}
        	else
        	{
        		$i = 0;
        		foreach ($familie as $familia) {
        			if($familia->ID_Familie == $aliance->Aliance1)
        			{
        				unset($familie[$i]);
        			}
        			$i++;
        		}
        	}
        }

        $iterator = 0;
        foreach ($familie as $familia) {
	        foreach ($notShow as $result) {
	        	if($familia->ID_Familie == $result->PozvankaOd || $familia->ID_Familie == $result->PozvankaPro)
	        		unset($familie[$iterator]);
	        }
        	$iterator++;
        }
        

        return view('show-alliance')->with('familie', $familie);
    }

    public function alianceRequest(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 4){
            return redirect('no-permission');
        }

        $request = new AlianceRequest();
        $request->ID_Familie1 = $_SESSION['familia'];
        $request->ID_Familie2 = $req->input('familia_id');
        $request->save();

        $familiaFrom = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$_SESSION['familia'])[0]->JmenoFamilie;
        $familiaTo = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$request->ID_Familie2)[0]->JmenoFamilie;

		$news = new newsModel();
		$news->date = date("Y-m-d H:i:s");
        $news->title = "Aliance";
        $news->content = "Familie $familiaFrom navrhnula alianci familii $familiaTo.";
        $news->save();	

        return redirect('show-alliance');
    }

    public function acceptAliance(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 4){
            return redirect('no-permission');
        }

        $removeRequest = DB::delete("DELETE FROM PozvankaDoAliance WHERE ID_Familie1 = ".$req->input('ID_Familie')." AND ID_Familie2 = ".$_SESSION['familia']);

        $aliance = new Aliance();
        $aliance->Aliance1 = $req->input('ID_Familie');
        $aliance->Aliance2 = $_SESSION['familia'];
        $aliance->DatumZacatku = date("Y-m-d H:i:s");
        $aliance->DatumUkonceni = date("Y-m-d H:i:s");
        $aliance->DatumUkonceni = date('Y-m-d H:i:s', strtotime("+1 month", strtotime($aliance->DatumUkonceni)));
        $aliance->save();

        $familiaFrom = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$_SESSION['familia'])[0]->JmenoFamilie;
        $familiaTo = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$req->input('ID_Familie'))[0]->JmenoFamilie;

        $news = new newsModel();
		$news->date = date("Y-m-d H:i:s");
        $news->title = "Aliance";
        $news->content = "Familie $familiaFrom přijmula požadavek familii $familiaTo.";
        $news->save();

        redirect('familia-editing');
    }

	public function declineAliance(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 4){
            return redirect('no-permission');
        }

        $removeRequest = DB::delete("DELETE FROM PozvankaDoAliance WHERE ID_Familie1 = ".$req->input('ID_Familie')." AND ID_Familie2 = ".$_SESSION['familia']);

		$familiaFrom = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$_SESSION['familia'])[0]->JmenoFamilie;
        $familiaTo = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$req->input('ID_Familie'))[0]->JmenoFamilie;

        $news = new newsModel();
		$news->date = date("Y-m-d H:i:s");
        $news->title = "Aliance";
        $news->content = "Familie $familiaFrom odmítnula požadavek familii $familiaTo.";
        $news->save();

        redirect('familia-editing');
    }
}
