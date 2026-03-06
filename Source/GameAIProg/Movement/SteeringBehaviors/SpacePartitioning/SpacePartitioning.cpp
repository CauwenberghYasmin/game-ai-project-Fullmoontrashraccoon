#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	float startX = -(Width / 2.0f);
	float startY = -(Height / 2.0f);
	
	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Cols; ++col)
		{
			float left = startX + (col * CellWidth);
			float bottom = startY + (row * CellHeight);
			
			Cells.push_back(Cell(left, bottom, CellWidth, CellHeight));
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	int index = PositionToIndex(Agent.GetPosition()); //use function :)
	Cells[index].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	int oldIndex = PositionToIndex(OldPos);
	int newIndex = PositionToIndex(Agent.GetPosition());
	
	if (oldIndex != newIndex)
	{
		// Remove from old cell
		auto& oldAgents = Cells[oldIndex].Agents;
		oldAgents.erase(std::remove(oldAgents.begin(), oldAgents.end(), &Agent), oldAgents.end()); //see childObject algo in prog4!!

		// Add to new cell
		Cells[newIndex].Agents.emplace_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius) //query = question or demand!
{
	NrOfNeighbors = 0; // reset
	FVector2D agentPos = Agent.GetPosition();
	
	FRect queryBox;	
	queryBox.Min = FVector2D(agentPos.X - QueryRadius, agentPos.Y - QueryRadius); //radius not diameter!
	queryBox.Max = FVector2D(agentPos.X + QueryRadius, agentPos.Y + QueryRadius);
	float sqrQueryRadius = QueryRadius * QueryRadius; //no extra work :>
	
	for (const Cell& cell : Cells)
	{
		if (DoRectsOverlap(queryBox, cell.BoundingBox))
		{
			for (ASteeringAgent* pPossibleNeighbour : cell.Agents)
			{
				if (pPossibleNeighbour != &Agent) //not itself as neighbour! 
				{
					float sqrDistance = FVector2D::DistSquared(agentPos, pPossibleNeighbour->GetPosition());
                    
					if (sqrDistance <= sqrQueryRadius)
					{
						Neighbors[NrOfNeighbors] = pPossibleNeighbour;
						NrOfNeighbors++;
					}
				}
			}
		}
	}
} //look into optinizing, these are a lot of loops!


void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	for (const Cell& cell : Cells)
	{
		FVector min(cell.BoundingBox.Min.X, cell.BoundingBox.Min.Y, 90.f);
		FVector max(cell.BoundingBox.Max.X, cell.BoundingBox.Max.Y, 90.f);
		DrawDebugBox(pWorld, (min + max) * 0.5f, (max - min) * 0.5f, FColor::Blue, false, -1.f, 0, 2.f);
		
		//text
		FVector textPos = (min + max) * 0.5f;
		DrawDebugString(pWorld, textPos, FString::FromInt(cell.Agents.size()), nullptr, FColor::White, 0.f, true, 1.5f);
	}
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.F, FColor{255, 0,0,255}, "draw getting called");
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	float relativeX = Pos.X + SpaceWidth /2 ;
	float relativeY = Pos.Y + SpaceHeight /2;
	
	int col = FMath::FloorToInt(relativeX / CellWidth);
	int row = FMath::FloorToInt(relativeY / CellHeight);

	//no crash
	col = FMath::Clamp(col, 0, NrOfCols - 1);
	row = FMath::Clamp(row, 0, NrOfRows - 1);
	
	return (row * NrOfCols) + col;

}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}