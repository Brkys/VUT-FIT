<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;
use App\criminal;
use App\familiaActionModel;
use Validator;

class CreateCriminalController extends Controller
{
    public function create(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else if($_SESSION['permission'] < 3 || $_SESSION['permission'] > 4){
            return redirect('no-permission');
        }
        else 
        {
            $lands = DB::select("SELECT ID_Uzemi, Adresa FROM Uzemi", [1]);

            return view('create-criminal')->with('lands', $lands);
        }
    }

    public function createCriminal(Request $req)
    {
        session_start();
        
        //pravidla pro validaci
        $rules = array(
            'activity_content' => 'required|max:60',
            'end_date' => 'required|date_format:Y-m-d'
        );

        //chceme ceske chybove hlasky
        $validatorMessagesCzech = array(
            'activity_content.required' => 'Vyplňte, prosím, typ činnosti.',
            'activity_content.max' => 'Typ činnosti má maximální povolenou délku 60 znaků.',
            'end_date.required' => 'Vyplňte, prosím, datum ukončení.',
            'end_date.date_format' => 'Toto není správný formát data.'
        );

        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else if($_SESSION['permission'] < 3 || $_SESSION['permission'] > 4){
            return redirect('no-permission');
        }
        else 
        {

            //odstranovani spatnych vstupu
            $type = $req->input('activity_content');
            $validatorType = Validator::make($req->only('activity_content'), ['activity_content' => 'required|max:60']);
            if($validatorType->fails()){
                $type = '';
            }

            $date = $req->input('end_date');
            $validatorDate = Validator::make($req->only('end_date'), ['end_date' => 'required|date_format:Y-m-d']);
            if($validatorDate->fails()){
                $date = '';
            }

            //naplneni zvalidovanych vstupu do pole, ktere se vrati uzivateli
            $validationResult = array(
                'activity_content' => $type,
                'end_date' => $date
            );

            //finalni validace - generovani chybovych hlasek, navraceni spravnych vstupu
            $validatorFinal = Validator::make($req->all(), $rules, $validatorMessagesCzech);
            if($validatorFinal->fails()){
                return redirect('create-criminal')->with('criminalInput', $validationResult)->withErrors($validatorFinal);
            }


            $criminal = new criminal();
            $criminal->ID_Uzemi = $req->input('land_id');
            $criminal->TypCinnosti = $req->input('activity_content');
            $criminal->DatumZacatku = date("Y-m-d");
            $criminal->DatumUkonceni = $req->input('end_date');
            $criminal->save();

            $familiaAction = new familiaActionModel();
            $familiaAction->ID_Familie = $_SESSION['familia'];
            $familiaAction->ID_Cinnosti = $criminal->ID_Cinnosti;
            $familiaAction->save();

            return redirect('create-criminal');
        }
    }

}
